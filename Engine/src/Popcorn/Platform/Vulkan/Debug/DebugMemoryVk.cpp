#ifdef PC_DEBUG

#include "Debug/DebugMemoryVk.h"
#include "Popcorn/Loaders/LoadersDefs.h"
#include <iomanip>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void *DebugDeviceMemoryVk::CreateStagingBuffer(
    VkDevice &device, VmaAllocator &vmaAllocator, VkBuffer &srcBuffer,
    VkDeviceSize size, VkCommandPool &commandPool, VkQueue &queue) {
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

  // Step 2: Copy from GPU buffer
  CopyBuffer(srcBuffer, m_stagingBuffer, size, device, commandPool, queue);

  // Step 3: Map and print
  vmaMapMemory(vmaAllocator, m_stagingAllocation, &m_data);

  return m_data;
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

void DebugDeviceMemoryVk::PrintVmaBuffer(VkDeviceSize size) {
  uint8_t *bytes = static_cast<uint8_t *>(m_data);
  for (size_t i = 0; i < size; ++i) {
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(bytes[i]) << " ";
    if ((i + 1) % 16 == 0)
      std::cout << std::endl;
  }

  std::cout << std::dec << std::endl; // Reset to decimal formatting
}

void DebugDeviceMemoryVk::DestroyStagingBuffer(VmaAllocator &allocator) {
  vmaUnmapMemory(allocator, m_stagingAllocation);
  vmaDestroyBuffer(allocator, m_stagingBuffer, m_stagingAllocation);
}

void DebugDeviceMemoryVk::PrintRawGpuVertexDataFromBytes(const void *rawVboData,
                                                         size_t vboSize,
                                                         const void *rawIboData,
                                                         size_t iboSize) {

  struct VertexTemp {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;

    std::string Print() const {
      std::stringstream ss;
      ss << "pos=(" << pos.x << ", " << pos.y << ", " << pos.z << "), "
         << "normal=(" << normal.x << ", " << normal.y << ", " << normal.z
         << "), "
         << "uv=(" << uv.x << ", " << uv.y << ")";
      return ss.str();
    }
  };
  // Ensure sizes are aligned
  size_t vertexSize = GltfVertexBufferLayout.strideValue; // Should be 32 bytes
  if (vboSize % vertexSize != 0 || iboSize % sizeof(uint32_t) != 0) {
    std::cerr << "Raw GPU buffer sizes are not aligned to expected "
                 "vertex/index formats!\n";
    return;
  }

  size_t vertexCount = vboSize / vertexSize;
  size_t indexCount = iboSize / sizeof(uint32_t);

  const VertexTemp *vertices = reinterpret_cast<const VertexTemp *>(rawVboData);
  const uint32_t *indices = reinterpret_cast<const uint32_t *>(rawIboData);

  std::cout << "\n=== DECODED GPU VERTEX DATA (" << vertexCount
            << " vertices) ===\n";
  for (size_t i = 0; i < vertexCount; ++i) {
    std::cout << "  Vertex " << i << ": " << vertices[i].Print() << "\n";
  }

  std::cout << "\n=== DECODED GPU INDEX DATA (" << indexCount
            << " indices) ===\n  ";
  for (size_t i = 0; i < indexCount; ++i) {
    std::cout << indices[i];
    if (i + 1 < indexCount)
      std::cout << " ";
  }
  std::cout << "\n";
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END

#endif
