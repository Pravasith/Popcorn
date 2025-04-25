#include "Memory/MemoryFactoryVk.h"
#include "BufferObjectsVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void MemoryFactoryVk::CreateAndAllocStagingBuffers(VkDeviceSize vboSize,
                                                   VkDeviceSize iboSize) {
  //
  // -----------------------------------------------------------------------
  // --- VERTEX BUFFERS CREATION -------------------------------------------

  VkBufferCreateInfo vboStagingInfo{};
  BufferVkUtils::GetDefaultVkBufferState(vboStagingInfo, vboSize);
  vboStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo vboStagingVmaInfo{};
  vboStagingVmaInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VkResult vboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &vboStagingInfo,
      &vboStagingVmaInfo, &m_submeshVBOsStaging, &m_submeshVBOsAllocStaging,
      nullptr);
  if (vboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create VBO staging buffer");
  };

  //
  // -----------------------------------------------------------------------
  // --- VERTEX BUFFERS MAP & COPY -----------------------------------------
  if (vmaMapMemory(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                   m_submeshVBOsAllocStaging,
                   &m_submeshVboMapping) != VK_SUCCESS) {
    throw std::runtime_error("Couldn't 'map' VBO staging buffer");
  };

  //
  //
  //
  //
  // -----------------------------------------------------------------------
  // --- INDEX BUFFERS CREATION --------------------------------------------
  VkBufferCreateInfo iboStagingInfo{};
  BufferVkUtils::GetDefaultVkBufferState(iboStagingInfo, iboSize);
  iboStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo iboStagingVmaInfo{};
  iboStagingVmaInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VkResult iboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &iboStagingInfo,
      &iboStagingVmaInfo, &m_submeshIBOsStaging, &m_submeshIBOsAllocStaging,
      nullptr);
  if (iboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create IBO staging buffer");
  };

  //
  // -----------------------------------------------------------------------
  // --- INDEX BUFFERS MAP & COPY ------------------------------------------
  if (vmaMapMemory(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                   m_submeshIBOsAllocStaging,
                   &m_submeshIboMapping) != VK_SUCCESS) {
    throw std::runtime_error("Couldn't map IBO staging buffer");
  };
}

void MemoryFactoryVk::FlushBuffersStagingToLocal(VkDeviceSize submeshVbosSize,
                                                 VkDeviceSize submeshIbosSize) {
  BufferVkUtils::CopyStagingToMainBuffers(m_submeshVBOsStaging, m_submeshVBOs,
                                          submeshVbosSize);
  BufferVkUtils::CopyStagingToMainBuffers(m_submeshIBOsStaging, m_submeshIBOs,
                                          submeshIbosSize);
};

void MemoryFactoryVk::CreateAndAllocLocalBuffers(VkDeviceSize vboSize,
                                                 VkDeviceSize iboSize) {
  //
  //
  //
  //
  // -----------------------------------------------------------------------
  // --- VERTEX BUFFERS CREATION -------------------------------------------
  VkBufferCreateInfo vboMainInfo{};
  BufferVkUtils::GetDefaultVkBufferState(vboMainInfo, vboSize);
  vboMainInfo.usage =
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo vboMainVmaInfo{};
  vboMainVmaInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkResult vboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &vboMainInfo,
      &vboMainVmaInfo, &m_submeshVBOs, &m_submeshVBOsAlloc, nullptr);
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
  BufferVkUtils::GetDefaultVkBufferState(iboMainInfo, iboSize);
  iboMainInfo.usage =
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo iboMainVmaInfo{};
  iboMainVmaInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkResult iboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &iboMainInfo,
      &iboMainVmaInfo, &m_submeshIBOs, &m_submeshIBOsAlloc, nullptr);
  if (iboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create IBO device buffer");
  };
}

void MemoryFactoryVk::CleanUpStagingBuffers() {
  auto &allocator = ContextVk::MemoryAllocator()->GetVMAAllocator();

  if (m_submeshVboMapping) {
    vmaUnmapMemory(allocator, m_submeshVBOsAllocStaging);
    m_submeshVboMapping = nullptr;
  }

  if (m_submeshIboMapping) {
    vmaUnmapMemory(allocator, m_submeshIBOsAllocStaging);
    m_submeshIboMapping = nullptr;
  }

  if (m_submeshVBOsStaging != VK_NULL_HANDLE && m_submeshVBOsAllocStaging) {
    vmaDestroyBuffer(allocator, m_submeshVBOsStaging,
                     m_submeshVBOsAllocStaging);
    m_submeshVBOsStaging = VK_NULL_HANDLE;
    m_submeshVBOsAllocStaging = nullptr;
  }

  if (m_submeshIBOsStaging != VK_NULL_HANDLE && m_submeshIBOsAllocStaging) {
    vmaDestroyBuffer(allocator, m_submeshIBOsStaging,
                     m_submeshIBOsAllocStaging);
    m_submeshIBOsStaging = VK_NULL_HANDLE;
    m_submeshIBOsAllocStaging = nullptr;
  }
};

void MemoryFactoryVk::CleanUpLocalBuffers() {
  auto &allocator = ContextVk::MemoryAllocator()->GetVMAAllocator();

  if (m_submeshVBOs != VK_NULL_HANDLE && m_submeshVBOsAlloc) {
    vmaDestroyBuffer(allocator, m_submeshVBOs, m_submeshVBOsAlloc);
    m_submeshVBOs = VK_NULL_HANDLE;
    m_submeshVBOsAlloc = nullptr;
  }

  if (m_submeshIBOs != VK_NULL_HANDLE && m_submeshIBOsAlloc) {
    vmaDestroyBuffer(allocator, m_submeshIBOs, m_submeshIBOsAlloc);
    m_submeshIBOs = VK_NULL_HANDLE;
    m_submeshIBOsAlloc = nullptr;
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
