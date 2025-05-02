#include "Memory/MemoryFactoryVk.h"
#include "BufferObjectsVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
#include "MaterialTypes.h"
#include "Memory/Helpers.h"
#include "RenderFlows/RenderFlowVk.h"
#include "Uniforms.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void MemoryFactoryVk::AllocVboIboStagingBuffers(VkDeviceSize vboSize,
                                                VkDeviceSize iboSize) {

  VkPhysicalDeviceProperties properties{};
  ContextVk::Device()->GetPhysicalDeviceProperties(properties);

  VkDeviceSize alignedVboSize =
      PC_AlignCeil(vboSize, properties.limits.optimalBufferCopyOffsetAlignment);
  VkDeviceSize alignedIboSize =
      PC_AlignCeil(iboSize, properties.limits.optimalBufferCopyOffsetAlignment);
  //
  // -----------------------------------------------------------------------
  // --- VERTEX BUFFERS CREATION -------------------------------------------
  VkBufferCreateInfo vboStagingInfo{};
  BufferVkUtils::GetDefaultVkBufferState(vboStagingInfo, alignedVboSize);
  vboStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo vboStagingVmaInfo{};
  vboStagingVmaInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VkResult vboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &vboStagingInfo,
      &vboStagingVmaInfo, &m_vboStaging, &m_vboStagingAlloc, nullptr);
  if (vboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create VBO staging buffer");
  };

  //
  // -----------------------------------------------------------------------
  // --- VERTEX BUFFERS MAP & COPY -----------------------------------------
  if (vmaMapMemory(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                   m_vboStagingAlloc, &m_vboStagingMapping) != VK_SUCCESS) {
    throw std::runtime_error("Couldn't 'map' VBO staging buffer");
  };

  //
  //
  //
  //
  // -----------------------------------------------------------------------
  // --- INDEX BUFFERS CREATION --------------------------------------------
  VkBufferCreateInfo iboStagingInfo{};
  BufferVkUtils::GetDefaultVkBufferState(iboStagingInfo, alignedIboSize);
  iboStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo iboStagingVmaInfo{};
  iboStagingVmaInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VkResult iboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &iboStagingInfo,
      &iboStagingVmaInfo, &m_iboStaging, &m_iboStagingAlloc, nullptr);
  if (iboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create IBO staging buffer");
  };

  //
  // -----------------------------------------------------------------------
  // --- INDEX BUFFERS MAP & COPY ------------------------------------------
  if (vmaMapMemory(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                   m_iboStagingAlloc, &m_iboStagingMapping) != VK_SUCCESS) {
    throw std::runtime_error("Couldn't map IBO staging buffer");
  };
}

void MemoryFactoryVk::FlushVBOsAndIBOsStagingToLocal(
    VkDeviceSize submeshVbosSize, VkDeviceSize submeshIbosSize) {
  VkPhysicalDeviceProperties properties{};
  ContextVk::Device()->GetPhysicalDeviceProperties(properties);
  VkDeviceSize alignedVboSize = PC_AlignCeil(
      submeshVbosSize, properties.limits.optimalBufferCopyOffsetAlignment);
  VkDeviceSize alignedIboSize = PC_AlignCeil(
      submeshIbosSize, properties.limits.optimalBufferCopyOffsetAlignment);

  BufferVkUtils::CopyStagingToMainBuffers(m_vboStaging, m_vbo, alignedVboSize);
  BufferVkUtils::CopyStagingToMainBuffers(m_iboStaging, m_ibo, alignedIboSize);
};

void MemoryFactoryVk::AllocVboIboLocalBuffers(VkDeviceSize vboSize,
                                              VkDeviceSize iboSize) {

  VkPhysicalDeviceProperties properties{};
  ContextVk::Device()->GetPhysicalDeviceProperties(properties);

  VkDeviceSize alignedVboSize =
      PC_AlignCeil(vboSize, properties.limits.optimalBufferCopyOffsetAlignment);
  VkDeviceSize alignedIboSize =
      PC_AlignCeil(iboSize, properties.limits.optimalBufferCopyOffsetAlignment);
  //
  //
  //
  //
  // -----------------------------------------------------------------------
  // --- VERTEX BUFFERS CREATION -------------------------------------------
  VkBufferCreateInfo vboMainInfo{};
  BufferVkUtils::GetDefaultVkBufferState(vboMainInfo, alignedVboSize);
  vboMainInfo.usage =
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo vboMainVmaInfo{};
  vboMainVmaInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkResult vboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &vboMainInfo,
      &vboMainVmaInfo, &m_vbo, &m_vboAlloc, nullptr);
  if (vboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create VBO device buffer");
  };

  //
  //
  //
  //
  // -----------------------------------------------------------------------
  // --- INDEX BUFFERS CREATION --------------------------------------------
  VkBufferCreateInfo iboMainInfo{};
  BufferVkUtils::GetDefaultVkBufferState(iboMainInfo, alignedIboSize);
  iboMainInfo.usage =
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo iboMainVmaInfo{};
  iboMainVmaInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkResult iboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &iboMainInfo,
      &iboMainVmaInfo, &m_ibo, &m_iboAlloc, nullptr);
  if (iboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create IBO device buffer");
  };
}

void MemoryFactoryVk::CleanUpVboIboStagingBuffers() {
  auto &allocator = ContextVk::MemoryAllocator()->GetVMAAllocator();

  if (m_vboStagingMapping) {
    vmaUnmapMemory(allocator, m_vboStagingAlloc);
    m_vboStagingMapping = nullptr;
  }

  if (m_iboStagingMapping) {
    vmaUnmapMemory(allocator, m_iboStagingAlloc);
    m_iboStagingMapping = nullptr;
  }

  if (m_vboStaging != VK_NULL_HANDLE && m_vboStagingAlloc) {
    vmaDestroyBuffer(allocator, m_vboStaging, m_vboStagingAlloc);
    m_vboStaging = VK_NULL_HANDLE;
    m_vboStagingAlloc = nullptr;
  }

  if (m_iboStaging != VK_NULL_HANDLE && m_iboStagingAlloc) {
    vmaDestroyBuffer(allocator, m_iboStaging, m_iboStagingAlloc);
    m_iboStaging = VK_NULL_HANDLE;
    m_iboStagingAlloc = nullptr;
  }
};

void MemoryFactoryVk::CleanUpVboIboLocalBuffers() {
  auto &allocator = ContextVk::MemoryAllocator()->GetVMAAllocator();

  if (m_vbo != VK_NULL_HANDLE && m_vboAlloc) {
    vmaDestroyBuffer(allocator, m_vbo, m_vboAlloc);
    m_vbo = VK_NULL_HANDLE;
    m_vboAlloc = nullptr;
  }

  if (m_ibo != VK_NULL_HANDLE && m_iboAlloc) {
    vmaDestroyBuffer(allocator, m_ibo, m_iboAlloc);
    m_ibo = VK_NULL_HANDLE;
    m_iboAlloc = nullptr;
  }
};

template <MaterialTypes T>
void MemoryFactoryVk::CopySubmeshGroupToVboIboStagingBuffers(
    SubmeshOffsets &accSubmeshGroupOffsets,
    const SubmeshGroups<T> &submeshGroups) {

  VkDeviceSize &vboOffsetRef = accSubmeshGroupOffsets.vboOffset;
  VkDeviceSize &iboOffsetRef = accSubmeshGroupOffsets.iboOffset;

  for (auto &[matId, submeshes] : submeshGroups) {
    for (Submesh<T> *submesh : submeshes) {
      //
      // VBOs ---------------------------------------------------------------
      const BufferDefs::Layout &vboLayout =
          submesh->GetVertexBuffer()->GetLayout();
      const VkDeviceSize vboSize = vboLayout.countValue * vboLayout.strideValue;
      memcpy((byte_t *)m_vboStagingMapping + vboOffsetRef,
             submesh->GetVertexBuffer()->GetBufferData(), (size_t)vboSize);

      //
      // IBOs ---------------------------------------------------------------
      IndexBuffer<uint32_t> *indexBuffer = submesh->GetIndexBuffer();
      const VkDeviceSize iboSize = indexBuffer->GetCount() * sizeof(uint32_t);
      memcpy((byte_t *)m_iboStagingMapping + iboOffsetRef,
             indexBuffer->GetBufferData(), (size_t)iboSize);

      //
      // Offsets ------------------------------------------------------------
      m_vboIboOffsets[matId].emplace_back(
          std::pair(vboOffsetRef, iboOffsetRef));

      vboOffsetRef += vboSize;
      iboOffsetRef += iboSize;
    }
  };
};

template <MaterialTypes T>
void MemoryFactoryVk::GetVboIboUboSizes(SubmeshGroups<T> &submeshGroups,
                                        VkDeviceSize &vboSize,
                                        VkDeviceSize &iboSize,
                                        VkDeviceSize &uboSize) {

  auto *device = ContextVk::Device();
  VkPhysicalDeviceProperties properties{};
  device->GetPhysicalDeviceProperties(properties);
  VkDeviceSize alignment = properties.limits.minUniformBufferOffsetAlignment;

  VkDeviceSize vbo = 0, ibo = 0;
  VkDeviceSize cameraUbo = 0, worldMatrixUbo = 0, basicMatUbo = 0,
               pbrMatUbo = 0,
               lightsUbo = 0; // ignore the lightsUbos for now

  for (auto &[matId, submeshes] : submeshGroups) {
    if constexpr (T == MaterialTypes::BasicMat) {
      basicMatUbo +=
          PC_AlignCeil(UniformDefs::BasicMaterialUniform::size, alignment);
    } else if constexpr (T == MaterialTypes::PbrMat) {
      pbrMatUbo +=
          PC_AlignCeil(UniformDefs::PbrMaterialUniform::size, alignment);
    }

    for (Submesh<T> *submesh : submeshes) {
      // --- VBO & IBO sizes -------------------------------------------------
      const BufferDefs::Layout &vboLayout =
          submesh->GetVertexBuffer()->GetLayout();
      vbo = vboLayout.countValue * vboLayout.strideValue;

      IndexBuffer<uint32_t> *indexBuffer = submesh->GetIndexBuffer();
      ibo = indexBuffer->GetCount() * sizeof(uint32_t);

      vboSize += vbo;
      iboSize += ibo;

      // --- UBO sizes -------------------------------------------------------
      worldMatrixUbo +=
          PC_AlignCeil(UniformDefs::GameObjectUniform::size, alignment);
    }
  };

  // --- UBO sizes -----------------------------------------------------------
  cameraUbo = PC_AlignCeil(UniformDefs::CameraUniform::size, alignment);
  uboSize += worldMatrixUbo + basicMatUbo + pbrMatUbo + cameraUbo;
}

// Template instantiation (explicit for linker visibility)
template void MemoryFactoryVk::CopySubmeshGroupToVboIboStagingBuffers<
    MaterialTypes::BasicMat>(
    SubmeshOffsets &accSubmeshOffsets,
    const std::unordered_map<MaterialHashType,
                             std::vector<Submesh<MaterialTypes::BasicMat> *>>
        &submeshGroups);

template void
MemoryFactoryVk::CopySubmeshGroupToVboIboStagingBuffers<MaterialTypes::PbrMat>(
    SubmeshOffsets &accSubmeshOffsets,
    const std::unordered_map<MaterialHashType,
                             std::vector<Submesh<MaterialTypes::PbrMat> *>>
        &submeshGroups);

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
