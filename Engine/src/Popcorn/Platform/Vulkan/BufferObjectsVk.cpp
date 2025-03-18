#include "BufferObjectsVk.h"
#include "BufferObjects.h"
#include "CommandPoolVk.h"
#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Helpers.h"
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

#include <glm/glm.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void VertexBufferVk::Bind() {

};

void VertexBufferVk::UnBind() {

};

void VertexBufferVk::GetDefaultVertexInputBindingDescription(
    VkVertexInputBindingDescription &bindingDescription,
    const BufferDefs::Layout &layout) {
  bindingDescription.binding = 0;
  bindingDescription.stride = layout.strideValue;
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
};

void VertexBufferVk::GetDefaultVertexInputAttributeDescriptions(
    std::vector<VkVertexInputAttributeDescription> &attrDescriptions,
    const BufferDefs::Layout &layout) {
  attrDescriptions.resize(layout.countValue);

  for (int i = 0; i < layout.countValue; ++i) {
    attrDescriptions[i].binding = 0;
    attrDescriptions[i].location = i;
    attrDescriptions[i].format =
        MapAttrTypeToVulkanType(layout.attrTypesValue[i]);
    attrDescriptions[i].offset = layout.attrOffsetsValue[i];
  };
};

//
//
// --- VULKAN BUFFER MEMORY UTILS -------------------------------------------
// --------------------------------------------------------------------------

void BufferVkUtils::RecordBindVkVertexBuffersCommand(
    const VkCommandBuffer &commandBuffer, VkBuffer *vkBuffer,
    VkDeviceSize *offsets, const uint32_t buffersCount) {
  vkCmdBindVertexBuffers(commandBuffer, 0, buffersCount, vkBuffer, offsets);
};

void BufferVkUtils::GetDefaultVkBufferState(VkBufferCreateInfo &bufferInfo,
                                            VkDeviceSize bufferSize) {
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = bufferSize;
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
};

void BufferVkUtils::AllocateVkBuffer(
    VkBuffer &vkBuffer, VkDeviceMemory &vkBufferMemory,
    const VkBufferCreateInfo &bufferInfo,
    const VkMemoryPropertyFlags memoryPropertyFlags) {

  auto *deviceVkStn = DeviceVk::Get();
  auto &device = deviceVkStn->GetDevice();

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &vkBuffer) != VK_SUCCESS) {
    std::runtime_error("Error creating  Buffer!");
  };

  //
  // QUERY MEMORY REQIREMENTS USING device AND vkBuffer
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, vkBuffer, &memRequirements);

  //
  // ALLOCATE MEMORY
  // TODO: Use VMA to allocate memory
  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = deviceVkStn->FindMemoryType(
      memRequirements.memoryTypeBits, memoryPropertyFlags);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &vkBufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error(
        "Failed to allocate memory for the vertex buffer!");
  };

  //
  // BIND MEMORY TO THE BUFFER
  vkBindBufferMemory(device, vkBuffer, vkBufferMemory, 0);
};

void *BufferVkUtils::MapVkMemoryToCPU(VkDeviceMemory &vkBufferMemory,
                                      VkDeviceSize beginOffset,
                                      VkDeviceSize endOffset) {
  auto &device = DeviceVk::Get()->GetDevice();

  //
  // FILL VERTEX BUFFER
  void *data;
  vkMapMemory(device, vkBufferMemory, beginOffset, endOffset, 0, &data);

  return data;
};

void BufferVkUtils::CopyBufferCPUToGPU(byte_t *destPtr, byte_t *srcPtr,
                                       VkDeviceSize size) {
  memcpy(destPtr, srcPtr, (size_t)size);
};

void BufferVkUtils::CopyBufferGPUToGPU(VkBuffer &srcBuffer, VkBuffer &dstBuffer,
                                       VkDeviceSize size) {
  auto &device = DeviceVk::Get()->GetDevice();
  auto *commandPoolVkStn = CommandPoolVk::Get();

  VkCommandBufferAllocateInfo allocInfo{};
  commandPoolVkStn->GetDefaultCommandBufferAllocInfo(allocInfo);

  VkCommandBuffer commandBuffer;
  commandPoolVkStn->AllocCommandBuffers(allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  //
  // Record command buffer
  {
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);
  }

  //
  // Submit command buffer
  {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    auto &graphicsQueue = DeviceVk::Get()->GetGraphicsQueue();

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
  }

  vkFreeCommandBuffers(device, commandPoolVkStn->GetVkCommandPool(), 1,
                       &commandBuffer);
};

void BufferVkUtils::UnmapVkMemoryFromCPU(VkDeviceMemory &vkBufferMemory) {
  auto &device = DeviceVk::Get()->GetDevice();
  vkUnmapMemory(device, vkBufferMemory);
};

void BufferVkUtils::DestroyVkBuffer(VkBuffer &vkBuffer,
                                    VkDeviceMemory &vkBufferMemory) {
  auto &device = DeviceVk::Get()->GetDevice();
  vkDestroyBuffer(device, vkBuffer, nullptr);
  vkFreeMemory(device, vkBufferMemory, nullptr);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
