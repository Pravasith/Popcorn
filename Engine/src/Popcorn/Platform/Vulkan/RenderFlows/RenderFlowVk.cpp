#include "RenderFlows/RenderFlowVk.h"
#include "ContextVk.h"
#include "Memory/Memory.h"
#include "Memory/MemoryFactoryVk.h"
#include "Popcorn/Core/Assert.h"
#include "SamplerVk.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// Game Object refs -------------------------------------------------------
MaterialSubmeshesMap<MaterialTypes::BasicMat>
    RenderFlowVk::s_basicSubmeshGroups{};
MaterialSubmeshesMap<MaterialTypes::PbrMat> RenderFlowVk::s_pbrSubmeshGroups{};

std::vector<Light *> RenderFlowVk::s_lights{};
std::vector<Camera *> RenderFlowVk::s_cameras{};
std::vector<Empty *> RenderFlowVk::s_emptys{}; // useless for now
                                               //

//
// Samplers ---------------------------------------------------------------
RenderFlowVk::SamplersVk RenderFlowVk::s_samplersVk{};

//
// Material values --------------------------------------------------------
MaterialMap<MaterialTypes::BasicMat> RenderFlowVk::s_basicMaterials;
MaterialMap<MaterialTypes::PbrMat> RenderFlowVk::s_pbrMaterials;

//
// ------------------------------------------------------------------------
uint64_t RenderFlowVk::s_submeshCount = 0;

void RenderFlowVk::AllocMemory() {
  // basicMat1 : [sm1, sm2, sm3, ... ]
  // basicMat2 : [sm1, sm2 ... ]

  // pbrMat1 : [sm1, sm2, sm3, ... ]
  // pbrMat2 : [sm1, sm2 ... ]

  auto *memoryFactory = ContextVk::MemoryFactory();

  // Extract -
  // 1. All vbos, ibos, ubos individual offsets (BufferOffsets)
  // 2. Ubo & ssbo bufferView sizes (not aligned)
  memoryFactory->ExtractOffsetsMaterialsSubmeshes(s_basicSubmeshGroups);
  memoryFactory->ExtractOffsetsMaterialsSubmeshes(s_pbrSubmeshGroups);
  memoryFactory->ExtractOffsetsLightsCamerasEmptys(s_lights, s_cameras,
                                                   s_emptys);

  // Aligns vbo & ibo (BufferViews) for optimal copy (staging -> local)
  memoryFactory->AlignVboIboBufferViews();

  // Calculates aligned ubo bufferView sizes (BufferViews)
  memoryFactory->CalculateUboSsboBaseOffsets();

  // Allocate vbo and ibo buffers (staging & local)
  memoryFactory->AllocSubmeshVboIboStaging();
  memoryFactory->AllocSubmeshVboIboLocal();

  // Allocate ubo buffers (local)
  memoryFactory
      ->AllocUboSsboLocalBuffers(); // clean up in RenderFlowVk::FreeMemory()

  // Fill submesh vbos, ibos and ubos, material ubos
  memoryFactory->FillBuffersMaterialsSubmeshes(s_basicSubmeshGroups);
  memoryFactory->FillBuffersMaterialsSubmeshes(s_pbrSubmeshGroups);
  memoryFactory->FillBuffersLightsCamerasEmptys(s_lights, s_cameras, s_emptys);

  PC_ASSERT(s_submeshCount, "Submesh count is zero.");

  // Copy the staging data to local buffers
  memoryFactory->FlushVboIboStagingToLocal();

  // Unmap, deallocate & destroy staging buffers
  memoryFactory->CleanUpSubmeshVboIboBuffersStaging();
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
  ContextVk::MemoryFactory()->CleanUpUboSsboLocalBuffers();
  ContextVk::MemoryFactory()
      ->CleanUpVboIboLocalBuffers(); // Deallocate & destroy
};

void RenderFlowVk::ProcessSceneUpdates(const uint32_t currentFrame) {
  // TODO: Copy from OLD
}

GFX_NAMESPACE_END ENGINE_NAMESPACE_END
