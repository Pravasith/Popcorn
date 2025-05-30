#include "RenderFlows/RenderFlowVk.h"
#include "CommonVk.h"
#include "ContextVk.h"
#include "MaterialTypes.h"
#include "Popcorn/Core/Assert.h"
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
};

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
  VkSamplerCreateInfo samplerInfo{};
  SamplerVk::GetDefaultSamplerCreateInfoValues(samplerInfo);
  samplerInfo.magFilter = VK_FILTER_NEAREST;
  samplerInfo.minFilter = VK_FILTER_NEAREST;
  s_samplersVk.frameSampler.Create(samplerInfo);
};

void RenderFlowVk::DestroySamplers() { s_samplersVk.frameSampler.Destroy(); };
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
