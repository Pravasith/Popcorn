#include "RenderFlows/RenderFlowVk.h"
#include "ContextVk.h"
#include "Memory/MemoryFactoryVk.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

PcBasicSubmeshGroups RenderFlowVk::s_basicSubmeshGroups{};
PcPbrSubmeshGroups RenderFlowVk::s_pbrSubmeshGroups{};
uint64_t RenderFlowVk::s_submeshCount = 0;

void RenderFlowVk::AllocateVMABuffers() {
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
  memoryFactory->CopySubmeshGroupToStagingBuffer<MaterialTypes::BasicMat>(
      groupOffsets, s_basicSubmeshGroups);
  memoryFactory->CopySubmeshGroupToStagingBuffer<MaterialTypes::PbrMat>(
      groupOffsets, s_pbrSubmeshGroups);

  //
  // Copy the staging data to local buffers ---------------------------------
  memoryFactory->AllocateLocalBuffers(s_submeshCount, s_submeshCount);
  memoryFactory->FlushBuffersStagingToMain(groupOffsets.submeshGroupVboSize,
                                           groupOffsets.submeshGroupIboSize);

  //
  // Unmap & destroy staging buffers ----------------------------------------
  memoryFactory->DeallocateStagingBuffers();

  // TODO: Allocate VMA VBOs, IBOs, and Material
  // - Device-local memory
  //      DONE -- One big VBO (has all submeshes VBOs)
  //      DONE -- One big IBO (has all submeshes IBOs)
  // - Host-visible memory
  //      DONE -- Staging: One big VBO (has all submeshes VBOs)
  //      DONE -- Staging: One big IBO (has all submeshes IBOs)
  //      -- One big BasicMaterial UBO (has all BasicMaterial UBOs)
  //      -- One big PbrMaterial UBO (has all PbrMaterial UBOs)
  //      -- One big ModelMatrix UBO (has all submeshes modelMatrix UBOs)
  //      -- One ViewProjMatrix UBO (has camera's UBOs)

  //
  // Memory creation example -- (basic materials)
  // TODO: Creation: Add ViewProj UBO (host-visible memory)
  // TODO: Later, in render loop -- Bind DescriptorSet -
  //       View-Proj-Matrix-DSet(offset)
  for (auto &[matId, submeshes] : s_basicSubmeshGroups) {
    // TODO: Creation: Add basic material UBO to big BasicMaterialUBO buffer
    //       (dynamic offsets, host-visible memory)
    // TODO: Later, in render loop -- Bind DescriptorSet -
    //       Basic-Material-DSet(offset)
    for (auto &submesh : submeshes) {
      // TODO: Creation: Add submesh VBO to big VBO buffer (dynamic offsets,
      //       local memory)
      // TODO: Creation: Add submesh IBO to big IBO buffer (dynamic offsets,
      //       local memory)
      // TODO: Creation: Add submesh ModelMatrix UBO to big IBO buffer
      //       (dynamic offsets, host-visible memory)
      // TODO: Later, in render loop
      //          -- Bind Submesh VBO:  VBO-Memory(offset),
      //          -- Bind DescriptorSet - SubmeshModelMatrixDSet(offset)
      //          -- Draw-Indexed Submesh: IBO-Memory(offset)
    }
  };

  // Pbr Material --
  for (auto &[matId, submeshes] : s_pbrSubmeshGroups) {
    for (auto &submesh : submeshes) {
    }
  };
};

GFX_NAMESPACE_END ENGINE_NAMESPACE_END
