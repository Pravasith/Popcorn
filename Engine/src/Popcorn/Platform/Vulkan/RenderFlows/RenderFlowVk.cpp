#include "RenderFlows/RenderFlowVk.h"
#include "Popcorn/Core/Assert.h"
#include "SamplerVk.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// Game Object refs -------------------------------------------------------
PcMaterialSubmeshesMap<MaterialTypes::BasicMat>
    RenderFlowVk::s_basicSubmeshGroups{};
PcMaterialSubmeshesMap<MaterialTypes::PbrMat>
    RenderFlowVk::s_pbrSubmeshGroups{};

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

PcRenderFlowImages<GBuffer> RenderFlowVk::s_gBufferImages{};
PcRenderFlowImages<Lighting> RenderFlowVk::s_lightingImages{};
PcRenderFlowImages<Composite> RenderFlowVk::s_compositeImages{};

void RenderFlowVk::AllocMemory() {
  // basicMat1 : [sm1, sm2, sm3, ... ]
  // basicMat2 : [sm1, sm2 ... ]

  // pbrMat1 : [sm1, sm2, sm3, ... ]
  // pbrMat2 : [sm1, sm2 ... ]

  auto *memory = ContextVk::Memory();

  // Extract -
  // 1. All vbos, ibos, ubos individual offsets (BufferOffsets)
  // 2. Ubo & ssbo bufferView sizes (not aligned)
  memory->ExtractOffsetsMaterialsSubmeshes(s_basicSubmeshGroups);
  memory->ExtractOffsetsMaterialsSubmeshes(s_pbrSubmeshGroups);
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
  memory->FillBuffersMaterialsSubmeshes(s_basicSubmeshGroups);
  memory->FillBuffersMaterialsSubmeshes(s_pbrSubmeshGroups);
  memory->FillBuffersLightsCamerasEmptys(s_lights, s_cameras, s_emptys);

  PC_ASSERT(s_submeshCount, "Submesh count is zero.");

  // Copy the staging data to local buffers
  memory->FlushVboIboStagingToLocal();

  // Unmap, deallocate & destroy staging buffers
  memory->CleanUpSubmeshVboIboBuffersStaging();
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

void RenderFlowVk::ProcessSceneUpdates(const uint32_t currentFrame) {
  // TODO: Copy from OLD
}

GFX_NAMESPACE_END ENGINE_NAMESPACE_END
