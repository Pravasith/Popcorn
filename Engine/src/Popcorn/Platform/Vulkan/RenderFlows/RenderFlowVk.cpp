#include "RenderFlows/RenderFlowVk.h"
#include "ContextVk.h"
#include "Memory/Helpers.h"
#include "Memory/MemoryFactoryVk.h"
#include "Popcorn/Core/Assert.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

BasicSubmeshGroups RenderFlowVk::s_basicSubmeshGroups{};
PbrSubmeshGroups RenderFlowVk::s_pbrSubmeshGroups{};

uint64_t RenderFlowVk::s_submeshCount = 0;

void RenderFlowVk::AllocVBOsAndIBOsMemory() {
  // basicMat1 : [sm1, sm2, sm3, ... ]
  // basicMat2 : [sm1, sm2 ... ]

  // pbrMat1 : [sm1, sm2, sm3, ... ]
  // pbrMat2 : [sm1, sm2 ... ]

  auto *memoryFactory = ContextVk::MemoryFactory();

  AccSubmeshBufferSizes submeshSizes{};

  // Basic submeshes size
  memoryFactory->GetAccSubmeshesBufferSizes(s_basicSubmeshGroups, submeshSizes);
  // Pbr submeshes size
  memoryFactory->GetAccSubmeshesBufferSizes(s_pbrSubmeshGroups, submeshSizes);

  VkPhysicalDeviceProperties properties{};
  ContextVk::Device()->GetPhysicalDeviceProperties(properties);

  // Align for optimal copy offset
  VkDeviceSize alignedVboSize = PC_AlignCeil(
      submeshSizes.vboSize, properties.limits.optimalBufferCopyOffsetAlignment);
  VkDeviceSize alignedIboSize = PC_AlignCeil(
      submeshSizes.iboSize, properties.limits.optimalBufferCopyOffsetAlignment);

  memoryFactory->AllocVboIboStagingBuffers(alignedVboSize, alignedIboSize);
  memoryFactory->AllocVboIboLocalBuffers(alignedVboSize, alignedIboSize);

  //
  // Copy VBO, IBO data to staging buffers ----------------------------------
  SubmeshOffsets submeshOffsets{};
  // Basic material submeshes
  memoryFactory->CopySubmeshGroupToVboIboStagingBuffers(submeshOffsets,
                                                        s_basicSubmeshGroups);
  // Pbr material submeshes
  memoryFactory->CopySubmeshGroupToVboIboStagingBuffers(submeshOffsets,
                                                        s_pbrSubmeshGroups);

  //
  // Copy the staging data to local buffers ---------------------------------
  PC_ASSERT(s_submeshCount, "Submesh count is zero.");
  memoryFactory->FlushVBOsAndIBOsStagingToLocal(alignedVboSize, alignedIboSize);
  memoryFactory->CleanUpVboIboStagingBuffers(); // Unmap, deallocate & destroy
};

void RenderFlowVk::AllocUBOsMemory() {
  auto *memoryFactory = ContextVk::MemoryFactory();
  AccGameObjectUboSizes gameObjectSizes{};

  VkPhysicalDeviceProperties properties{};
  ContextVk::Device()->GetPhysicalDeviceProperties(properties);

  // Get aligned ubo sizes
  memoryFactory->GetAccGameObjectsBufferSizes(s_cameras, gameObjectSizes,
                                              properties);
  memoryFactory->GetAccGameObjectsBufferSizes(s_emptys, gameObjectSizes,
                                              properties);
  memoryFactory->GetAccGameObjectsBufferSizes(s_lights, gameObjectSizes,
                                              properties);

  memoryFactory->AllocUboLocalBuffers(gameObjectSizes);
};

void RenderFlowVk::FreeUBOsMemory() {
  ContextVk::MemoryFactory()->CleanUpUboBuffers();
};

void RenderFlowVk::FreeVBOsAndIBOsMemory() {
  ContextVk::MemoryFactory()
      ->CleanUpVboIboLocalBuffers(); // Deallocate & destroy
};

void RenderFlowVk::ProcessSceneUpdates(const uint32_t currentFrame) {
  // TODO: Copy from OLD
}

GFX_NAMESPACE_END ENGINE_NAMESPACE_END
