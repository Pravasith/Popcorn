#include "RenderFlowVk.h"
#include "CommonVk.h"
#include "ContextVk.h"
#include "MaterialTypes.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "SamplerVk.h"
#include "Shader.h"
#include "Sources.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// Game Object refs -------------------------------------------------------
PcMaterialSubmeshesMap<MaterialTypes::BasicMat>
    RenderFlowVk::s_basicMatSubmeshesMap{};
PcMaterialSubmeshesMap<MaterialTypes::PbrMat>
    RenderFlowVk::s_pbrMatSubmeshesMap{};

std::vector<Light *> RenderFlowVk::s_lights{};
std::vector<Camera *> RenderFlowVk::s_cameras{};
std::vector<Empty *> RenderFlowVk::s_emptys{}; // useless for now
                                               //

//
// Samplers ---------------------------------------------------------------
RenderFlowVk::SamplersVk RenderFlowVk::s_samplersVk{};

//
// Material values --------------------------------------------------------
PcMaterialMap<MaterialTypes::BasicMat> RenderFlowVk::s_basicMaterials;
PcMaterialMap<MaterialTypes::PbrMat> RenderFlowVk::s_pbrMaterials;

//
// ------------------------------------------------------------------------
uint64_t RenderFlowVk::s_submeshCount = 0;

PcRenderFlowImages<GBuffer, MAX_FRAMES_IN_FLIGHT>
    RenderFlowVk::s_gBufferImages{};
PcRenderFlowImages<Lighting, MAX_FRAMES_IN_FLIGHT>
    RenderFlowVk::s_lightingImages{};
PcRenderFlowImages<Composite> RenderFlowVk::s_compositeImages{};

RenderFlowVk::DescriptorSetsVkStatic RenderFlowVk::s_commonDescriptorSets{};

RenderFlowVk::RenderFlowVk() {
  PC_PRINT("CREATED", TagType::Constr, "RenderFlowVk")
};
RenderFlowVk::~RenderFlowVk() {
  PC_PRINT("DESTROYED", TagType::Destr, "RenderFlowVk")
};

//
// --- METHODS -----------------------------------------------------------
void RenderFlowVk::AllocShaders() {
  //
  // Load all shaders ----------------------------------------------------
  ShaderLibrary *shaders = ContextVk::Shaders();
  shaders->LoadShaders<RendererType::Vulkan>();

  //
  // Basic mat shaders ---------------------------------------------------
  Buffer *basicMatShaderVert =
      &shaders->GetShader<RendererType::Vulkan, ShaderFiles::BasicMat_Vert>();
  Buffer *basicMatShaderFrag =
      &shaders->GetShader<RendererType::Vulkan, ShaderFiles::BasicMat_Frag>();
  Material<MaterialTypes::BasicMat>::SetShader<ShaderStages::VertexBit>(
      basicMatShaderVert);
  Material<MaterialTypes::BasicMat>::SetShader<ShaderStages::FragmentBit>(
      basicMatShaderFrag);

  //
  // Pbr mat shaders -----------------------------------------------------
  Buffer *pbrMatShaderVert =
      &shaders->GetShader<RendererType::Vulkan, ShaderFiles::PbrMat_Vert>();
  Buffer *pbrMatShaderFrag =
      &shaders->GetShader<RendererType::Vulkan, ShaderFiles::PbrMat_Frag>();
  Material<MaterialTypes::PbrMat>::SetShader<ShaderStages::VertexBit>(
      pbrMatShaderVert);
  Material<MaterialTypes::PbrMat>::SetShader<ShaderStages::FragmentBit>(
      pbrMatShaderFrag);
};

void RenderFlowVk::FreeShaders() {
  ShaderLibrary *shaders = ContextVk::Shaders();
  shaders->UnloadShaders<RendererType::Vulkan>();
}

void RenderFlowVk::AllocDescriptorsGlobal() {
  auto *layouts = ContextVk::DescriptorLayouts();
  auto *pools = ContextVk::DescriptorPools();
  auto *device = ContextVk::Device();
  auto *memory = ContextVk::Memory();

  VkPhysicalDeviceProperties properties{};
  device->GetPhysicalDeviceProperties(properties);

  DPoolVk &globalDescriptorsPool =
      pools->GetPool<DescriptorPools::GlobalDescriptorsPool>(
          MAX_FRAMES_IN_FLIGHT); // Creates pool if it
                                 // doesn't exist

  VkDescriptorSetLayout &cameraLayout =
      layouts->GetLayout<DescriptorSets::CameraSet>();
  std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> cameraLayouts{};
  std::fill(cameraLayouts.begin(), cameraLayouts.end(), cameraLayout);

  // Descriptor set will be cleaned automatically when pools are destroyed
  s_commonDescriptorSets.cameraSets =
      globalDescriptorsPool.AllocateDescriptorSets<DescriptorSets::CameraSet,
                                                   MAX_FRAMES_IN_FLIGHT>(
          device->GetDevice(), cameraLayouts);

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    PC_PRINT("ALLOCATED cameraSets["
                 << i << "] = " << (void *)s_commonDescriptorSets.cameraSets[i],
             TagType::Print, "GLOBAL DSET")
  }
}

void RenderFlowVk::UpdateDescriptorSetsGlobal() {
  auto *device = ContextVk::Device();
  auto *memory = ContextVk::Memory();

  VkPhysicalDeviceProperties properties{};
  device->GetPhysicalDeviceProperties(properties);

  // Bind sets with buffers
  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    VkDescriptorBufferInfo cameraBufferInfo{};
    cameraBufferInfo.buffer = memory->GetUboSet(i);
    cameraBufferInfo.offset = memory->GetBufferViews().camerasUbo.offset;
    cameraBufferInfo.range = memory->GetBufferViews().camerasUbo.alignedSize;
    // DescriptorLayoutsVk::GetDescriptorBufferRange<
    // DescriptorSets::CameraSet>(properties.limits);

    VkWriteDescriptorSet cameraWrite{};
    cameraWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    cameraWrite.dstSet = s_commonDescriptorSets.cameraSets[i];
    cameraWrite.dstBinding = 0;
    cameraWrite.dstArrayElement = 0;
    cameraWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    cameraWrite.descriptorCount = 1;
    cameraWrite.pBufferInfo = &cameraBufferInfo;
    cameraWrite.pImageInfo = nullptr;
    cameraWrite.pTexelBufferView = nullptr;

    std::vector<VkWriteDescriptorSet> writes{cameraWrite};

    vkUpdateDescriptorSets(device->GetDevice(), writes.size(), writes.data(), 0,
                           nullptr);
  }
}

void RenderFlowVk::AllocMemory() {
  // basicMat1 : [sm1, sm2, sm3, ... ]
  // basicMat2 : [sm1, sm2 ... ]

  // pbrMat1 : [sm1, sm2, sm3, ... ]
  // pbrMat2 : [sm1, sm2 ... ]

  auto *memory = ContextVk::Memory();

  // Extract -
  // 1. All vbos, ibos, ubos individual offsets (BufferOffsets)
  // 2. Ubo & ssbo bufferView sizes (not aligned)
  memory->ExtractOffsetsMaterialsSubmeshes(s_basicMatSubmeshesMap);
  memory->ExtractOffsetsMaterialsSubmeshes(s_pbrMatSubmeshesMap);
  memory->ExtractOffsetsLightsCamerasEmptys(s_lights, s_cameras, s_emptys);

  // Aligns vbo & ibo (BufferViews) for optimal copy (staging -> local)
  memory->AlignVboIboBufferViews();

  // Calculates aligned ubo bufferView sizes (BufferViews)
  memory->CalculateUboSsboBaseOffsets();

  // Allocate vbo and ibo buffers (staging & local)
  memory->AllocSubmeshVboIboStaging();
  memory->AllocSubmeshVboIboLocal();

  // Allocate ubo buffers (local)
  memory->AllocUboSsboLocalBuffers(); // clean up in RenderFlowVk::FreeMemory()

  // Fill submesh vbos, ibos and ubos, material ubos
  memory->FillVbosIbosUbosSubmeshMaterial(s_basicMatSubmeshesMap,
                                          s_basicMaterials);
  memory->FillVbosIbosUbosSubmeshMaterial(s_pbrMatSubmeshesMap, s_pbrMaterials);

  PC_ASSERT(s_submeshCount, "Submesh count is zero.");

  // Copy the staging data to local buffers
  memory->FlushVboIboStagingToLocal();

  // Unmap, deallocate & destroy staging buffers
  memory->CleanUpSubmeshVboIboBuffersStaging();
};

void RenderFlowVk::CopyDynamicUniformsToMemory(const uint32_t currentFrame) {
  auto *memory = ContextVk::Memory();

  memory->FillUbosSubmesh(s_basicMatSubmeshesMap, currentFrame);
  memory->FillUbosSubmesh(s_pbrMatSubmeshesMap, currentFrame);

  memory->FillUbosSsbosLightCameraEmpty(s_lights, s_cameras, s_emptys,
                                        currentFrame);
};

void RenderFlowVk::CreateSamplers() {
  // Color sampler
  VkSamplerCreateInfo colorSamplerInfo{};
  SamplerVk::GetDefaultSamplerCreateInfoValues(colorSamplerInfo);
  colorSamplerInfo.magFilter = VK_FILTER_NEAREST;
  colorSamplerInfo.minFilter = VK_FILTER_NEAREST;
  s_samplersVk.colorSampler.Create(colorSamplerInfo);

  // Depth sampler
  VkSamplerCreateInfo depthSamplerInfo{};
  SamplerVk::GetDefaultSamplerCreateInfoValues(depthSamplerInfo);
  depthSamplerInfo.compareEnable = VK_FALSE; // No shadow comparison
  depthSamplerInfo.magFilter = VK_FILTER_NEAREST;
  depthSamplerInfo.minFilter = VK_FILTER_NEAREST;
  depthSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  depthSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  depthSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  depthSamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  depthSamplerInfo.unnormalizedCoordinates = VK_FALSE;
  s_samplersVk.depthSampler.Create(depthSamplerInfo);
};

void RenderFlowVk::DestroySamplers() { s_samplersVk.colorSampler.Destroy(); };

void RenderFlowVk::FreeMemory() {
  ContextVk::Memory()->CleanUpUboSsboLocalBuffers();
  ContextVk::Memory()->CleanUpVboIboLocalBuffers();
};

template void RenderFlowVk::RegisterMaterialAndSubmesh(
    Submesh<MaterialTypes::BasicMat> *submesh);
template void RenderFlowVk::RegisterMaterialAndSubmesh(
    Submesh<MaterialTypes::PbrMat> *submesh);
template <MaterialTypes T>
void RenderFlowVk::RegisterMaterialAndSubmesh(Submesh<T> *submesh) {
  if constexpr (T == MaterialTypes::BasicMat) {
    uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
    if (PC_ValidateAndAddSubmesh(submesh, s_basicMatSubmeshesMap[materialId])) {
      s_basicMaterials[materialId] = submesh->GetMaterial();
      ++s_submeshCount;
    };
  } else if constexpr (T == MaterialTypes::PbrMat) {
    uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
    if (PC_ValidateAndAddSubmesh(submesh, s_pbrMatSubmeshesMap[materialId])) {
      s_pbrMaterials[materialId] = submesh->GetMaterial();
      ++s_submeshCount;
    };
  }
};

template void RenderFlowVk::UnregisterMaterialAndSubmesh(
    Submesh<MaterialTypes::BasicMat> *submesh);
template void RenderFlowVk::UnregisterMaterialAndSubmesh(
    Submesh<MaterialTypes::PbrMat> *submesh);
template <MaterialTypes T>
void RenderFlowVk::UnregisterMaterialAndSubmesh(Submesh<T> *submesh) {
  if constexpr (T == MaterialTypes::BasicMat) {
    uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
    if (PC_ValidateAndRemoveSubmesh(submesh,
                                    s_basicMatSubmeshesMap[materialId])) {
      --s_submeshCount;
    };
  } else if constexpr (T == MaterialTypes::PbrMat) {
    uint32_t materialId = PC_HashMaterialGroups(submesh->GetMaterial());
    if (PC_ValidateAndRemoveSubmesh(submesh,
                                    s_pbrMatSubmeshesMap[materialId])) {
      --s_submeshCount;
    };
  }
};

GFX_NAMESPACE_END ENGINE_NAMESPACE_END
