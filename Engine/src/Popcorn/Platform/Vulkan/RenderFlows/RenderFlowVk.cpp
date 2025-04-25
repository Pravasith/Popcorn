#include "RenderFlows/RenderFlowVk.h"
#include "ContextVk.h"
#include "Memory/MemoryFactoryVk.h"
#include "Popcorn/Core/Assert.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

PcBasicSubmeshGroups RenderFlowVk::s_basicSubmeshGroups{};
PcPbrSubmeshGroups RenderFlowVk::s_pbrSubmeshGroups{};
uint64_t RenderFlowVk::s_submeshCount = 0;

void RenderFlowVk::CreateAndAllocateVBOsAndIBOs() {
  // basicMat1 : [sm1, sm2, sm3, ... ]
  // basicMat2 : [sm1, sm2 ... ]

  // pbrMat1 : [sm1, sm2, sm3, ... ]
  // pbrMat2 : [sm1, sm2 ... ]
  //
  auto *memoryFactory = ContextVk::MemoryFactory();
  memoryFactory->CreateAndAllocStagingBuffers(s_submeshCount, s_submeshCount);
  memoryFactory->CreateAndAllocLocalBuffers(s_submeshCount, s_submeshCount);

  //
  // Copy VBO, IBO data to staging buffers ----------------------------------
  PcSubmeshGroupOffsets groupOffsets{};
  // Basic material submesh groups
  memoryFactory->CopySubmeshGroupToStagingBuffers(groupOffsets,
                                                  s_basicSubmeshGroups);
  // Pbr material submesh groups
  memoryFactory->CopySubmeshGroupToStagingBuffers(groupOffsets,
                                                  s_pbrSubmeshGroups);

  //
  // Copy the staging data to local buffers ---------------------------------
  PC_ASSERT(s_submeshCount, "Submesh count is zero.");

  memoryFactory->FlushBuffersStagingToLocal(groupOffsets.submeshGroupVboSize,
                                            groupOffsets.submeshGroupIboSize);

  memoryFactory->CleanUpStagingBuffers(); // Unmap, deallocate & destroy
};

void RenderFlowVk::DestroySubmeshPrimitiveBuffers() {
  auto *memoryFactory = ContextVk::MemoryFactory();
  memoryFactory->CleanUpLocalBuffers(); // Deallocate & destroy
};

void RenderFlowVk::ProcessSceneUpdates(const uint32_t currentFrame) {
  // TODO: Copy from OLD
}

GFX_NAMESPACE_END ENGINE_NAMESPACE_END
