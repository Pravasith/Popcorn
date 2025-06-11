#ifdef PC_DEBUG

#include "Debug/DebugMemoryVk.h"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

DebugDeviceMemoryVk *DebugDeviceMemoryVk::s_instance = nullptr;

void DebugDeviceMemoryVk::CreateStagingBuffer(VmaAllocator &vmaAllocator,
                                              VkDeviceSize size) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
  allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                    VMA_ALLOCATION_CREATE_MAPPED_BIT;

  vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocInfo, &m_stagingBuffer,
                  &m_stagingAllocation, nullptr);
};

void DebugDeviceMemoryVk::CopyBuffer(VkBuffer &srcBuffer, VkBuffer &dstBuffer,
                                     VkDeviceSize size, VkDevice &device,
                                     VkCommandPool &commandPool,
                                     VkQueue &queue) {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkResult res;

  VkCommandBuffer commandBuffer;
  res = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
  PC_ASSERT(res == VK_SUCCESS, "Failed to allocate command buffer");

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  res = vkBeginCommandBuffer(commandBuffer, &beginInfo);
  PC_ASSERT(res == VK_SUCCESS, "Failed to begin command buffer");

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  res = vkEndCommandBuffer(commandBuffer);
  PC_ASSERT(res == VK_SUCCESS, "Failed to end command buffer");

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  res = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
  PC_ASSERT(res == VK_SUCCESS, "Failed to submit queue");

  vkQueueWaitIdle(queue);
  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void DebugDeviceMemoryVk::PrintVmaBuffer(VkDevice &device,
                                         VmaAllocator &allocator,
                                         VkBuffer &srcBuffer, VkDeviceSize size,
                                         VkCommandPool &commandPool,
                                         VkQueue &queue) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
  allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                    VMA_ALLOCATION_CREATE_MAPPED_BIT;

  vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &m_stagingBuffer,
                  &m_stagingAllocation, nullptr);

  // Step 2: Copy from GPU buffer
  CopyBuffer(srcBuffer, m_stagingBuffer, size, device, commandPool, queue);

  // Step 3: Map and print
  void *data;
  vmaMapMemory(allocator, m_stagingAllocation, &data);

  uint8_t *bytes = static_cast<uint8_t *>(data);
  for (size_t i = 0; i < size; ++i) {
    printf("%02X ", bytes[i]);
    if ((i + 1) % 16 == 0)
      printf("\n");
  }

  vmaUnmapMemory(allocator, m_stagingAllocation);
  vmaDestroyBuffer(allocator, m_stagingBuffer, m_stagingAllocation);
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END

#endif
