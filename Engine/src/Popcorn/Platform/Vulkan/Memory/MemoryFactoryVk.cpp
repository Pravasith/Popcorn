#include "Memory/MemoryFactoryVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void MemoryFactoryVk::AllocateStagingBuffers(VkDeviceSize vboSize,
                                             VkDeviceSize iboSize) {
  //
  // -----------------------------------------------------------------------
  // --- VERTEX BUFFERS CREATION -------------------------------------------
  VkBuffer vboStagingBuffer{};
  VmaAllocation vboStagingAlloc{};

  VkBufferCreateInfo vboStagingInfo{};
  BufferVkUtils::GetDefaultVkBufferState(vboStagingInfo, vboSize);
  vboStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo vboStagingVmaInfo{};
  vboStagingVmaInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VkResult vboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &vboStagingInfo,
      &vboStagingVmaInfo, &vboStagingBuffer, &vboStagingAlloc, nullptr);
  if (vboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create VBO staging buffer");
  };

  //
  // -----------------------------------------------------------------------
  // --- VERTEX BUFFERS MAP & COPY -----------------------------------------
  if (vmaMapMemory(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                   vboStagingAlloc, &m_submeshVboMapping) != VK_SUCCESS) {
    throw std::runtime_error("Couldn't 'map' VBO staging buffer");
  };

  //
  //
  //
  //
  // -----------------------------------------------------------------------
  // --- INDEX BUFFERS CREATION --------------------------------------------
  VkBuffer iboStagingBuffer{};
  VmaAllocation iboStagingAlloc{};

  VkBufferCreateInfo iboStagingInfo{};
  BufferVkUtils::GetDefaultVkBufferState(iboStagingInfo, iboSize);
  iboStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  VmaAllocationCreateInfo iboStagingVmaInfo{};
  iboStagingVmaInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VkResult iboResult = vmaCreateBuffer(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &iboStagingInfo,
      &iboStagingVmaInfo, &iboStagingBuffer, &iboStagingAlloc, nullptr);
  if (iboResult != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create IBO staging buffer");
  };

  //
  // -----------------------------------------------------------------------
  // --- INDEX BUFFERS MAP & COPY ------------------------------------------
  if (vmaMapMemory(ContextVk::MemoryAllocator()->GetVMAAllocator(),
                   iboStagingAlloc, &m_submeshIboMapping) != VK_SUCCESS) {
    throw std::runtime_error("Couldn't map IBO staging buffer");
  };
}

void MemoryFactoryVk::AllocateLocalBuffers(VkDeviceSize vboSize,
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

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
