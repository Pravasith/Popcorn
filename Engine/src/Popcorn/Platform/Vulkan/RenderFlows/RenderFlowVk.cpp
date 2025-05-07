#include "RenderFlows/RenderFlowVk.h"
#include "ContextVk.h"
#include "Memory/MemoryFactoryVk.h"
#include "Popcorn/Core/Assert.h"
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
  VkPhysicalDeviceProperties properties{};
  ContextVk::Device()->GetPhysicalDeviceProperties(properties);

  // Basic submeshes size
  memoryFactory->ExtractMaterialSubmeshOffsets(s_basicSubmeshGroups);
  // Pbr submeshes size
  memoryFactory->ExtractMaterialSubmeshOffsets(s_pbrSubmeshGroups);

  // Align for optimal copy offset
  VkDeviceSize alignedVboSize = PC_AlignCeil(
      submeshSizes.vboSize, properties.limits.optimalBufferCopyOffsetAlignment);
  VkDeviceSize alignedIboSize = PC_AlignCeil(
      submeshSizes.iboSize, properties.limits.optimalBufferCopyOffsetAlignment);

  memoryFactory->AllocVboIboStagingBuffers(alignedVboSize, alignedIboSize);
  memoryFactory->AllocVboIboLocalBuffers(alignedVboSize, alignedIboSize);

  memoryFactory->FillMaterialAndSubmeshBuffers(s_basicSubmeshGroups,
                                               submeshOffsets, materialOffsets);
  memoryFactory->FillMaterialAndSubmeshBuffers(s_pbrSubmeshGroups,
                                               submeshOffsets, materialOffsets);

  //
  // Copy the staging data to local buffers ---------------------------------
  PC_ASSERT(s_submeshCount, "Submesh count is zero.");
  memoryFactory->FlushVBOsAndIBOsStagingToLocal(alignedVboSize, alignedIboSize);
  memoryFactory->CleanUpVboIboStagingBuffers(); // Unmap, deallocate & destroy

  AccGameObjectUboSizes gameObjectSizes{};
  ContextVk::Device()->GetPhysicalDeviceProperties(properties);

  // Cameras ubos
  memoryFactory->GetAccGameObjectsBufferSizes(s_cameras, gameObjectSizes,
                                              properties);
  // Emptys ubos
  memoryFactory->GetAccGameObjectsBufferSizes(s_emptys, gameObjectSizes,
                                              properties);
  // Lights ubos
  memoryFactory->GetAccGameObjectsBufferSizes(s_lights, gameObjectSizes,
                                              properties);

  //
  // Allocate ubo buffers
  memoryFactory->AllocUboLocalBuffers(submeshSizes, gameObjectSizes);
};

void RenderFlowVk::FreeMemory() {
  ContextVk::MemoryFactory()->CleanUpUboBuffers();
  ContextVk::MemoryFactory()
      ->CleanUpVboIboLocalBuffers(); // Deallocate & destroy
};

void RenderFlowVk::ProcessSceneUpdates(const uint32_t currentFrame) {
  // TODO: Copy from OLD
}

GFX_NAMESPACE_END ENGINE_NAMESPACE_END
