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

void RenderFlowVk::AllocSubmeshPrimitiveBuffers() {
  // basicMat1 : [sm1, sm2, sm3, ... ]
  // basicMat2 : [sm1, sm2 ... ]

  // pbrMat1 : [sm1, sm2, sm3, ... ]
  // pbrMat2 : [sm1, sm2 ... ]
  //
  auto *memoryFactory = ContextVk::MemoryFactory();
  memoryFactory->AllocateStagingBuffers(s_submeshCount, s_submeshCount);

  //
  // Copy VBO, IBO data to staging buffers ----------------------------------
  PcSubmeshGroupOffsets groupOffsets{};
  // Basic material submesh groups
  memoryFactory->CopySubmeshGroupToStagingBuffer(groupOffsets,
                                                 s_basicSubmeshGroups);
  // Pbr material submesh groups
  memoryFactory->CopySubmeshGroupToStagingBuffer(groupOffsets,
                                                 s_pbrSubmeshGroups);

  //
  // Copy the staging data to local buffers ---------------------------------
  PC_ASSERT(s_submeshCount, "Submesh count is zero.");

  memoryFactory->AllocateLocalBuffers(s_submeshCount, s_submeshCount);
  memoryFactory->FlushBuffersStagingToMain(groupOffsets.submeshGroupVboSize,
                                           groupOffsets.submeshGroupIboSize);

  //
  // Unmap & destroy staging buffers ----------------------------------------
  memoryFactory->DeallocateStagingBuffers();
};

void RenderFlowVk::ProcessSceneUpdates(const uint32_t currentFrame) {
  // TODO: Copy from OLD
}

GFX_NAMESPACE_END ENGINE_NAMESPACE_END
