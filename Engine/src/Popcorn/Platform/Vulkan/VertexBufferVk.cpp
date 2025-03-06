#include "VertexBufferVk.h"
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

void VertexBufferVk::RecordBindVkBuffersCommand(
    const VkCommandBuffer &commandBuffer, VkBuffer *vkVertexBuffer,
    VkDeviceSize *offsets, const uint32_t vertexBuffersCount) {
  vkCmdBindVertexBuffers(commandBuffer, 0, vertexBuffersCount, vkVertexBuffer,
                         offsets);
};

void VertexBufferVk::GetDefaultVkBufferState(VkBufferCreateInfo &bufferInfo,
                                             VkDeviceSize bufferSize) {
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = bufferSize;
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
};

void VertexBufferVk::AllocateVkBuffer(
    VkBuffer &vkVertexBuffer, VkDeviceMemory &vkVertexBufferMemory,
    const VkBufferCreateInfo &bufferInfo,
    const VkMemoryPropertyFlags memoryPropertyFlags) {

  auto *deviceVkStn = DeviceVk::Get();
  auto &device = deviceVkStn->GetDevice();

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &vkVertexBuffer) !=
      VK_SUCCESS) {
    std::runtime_error("Error creating Vertex Buffer!");
  };

  //
  // QUERY MEMORY REQIREMENTS USING device AND vkVertexBuffer
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, vkVertexBuffer, &memRequirements);

  //
  // ALLOCATE MEMORY
  // TODO: Use VMA to allocate memory
  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = deviceVkStn->FindMemoryType(
      memRequirements.memoryTypeBits, memoryPropertyFlags);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &vkVertexBufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error(
        "Failed to allocate memory for the vertex buffer!");
  };

  //
  // BIND MEMORY TO THE BUFFER
  vkBindBufferMemory(device, vkVertexBuffer, vkVertexBufferMemory, 0);
};

void *VertexBufferVk::MapVkMemoryToCPU(VkDeviceMemory &vkVertexBufferMemory,
                                       VkDeviceSize beginOffset,
                                       VkDeviceSize endOffset) {
  auto &device = DeviceVk::Get()->GetDevice();
  //
  // FILL VERTEX BUFFER
  void *data;
  vkMapMemory(device, vkVertexBufferMemory, beginOffset, endOffset, 0, &data);

  return data;
};

void VertexBufferVk::CopyBufferCPUToGPU(VkDeviceMemory &vkVertexBufferMemory,
                                        byte_t *destPtr, byte_t *srcPtr,
                                        VkDeviceSize size) {
  auto &device = DeviceVk::Get()->GetDevice();
  memcpy(destPtr, srcPtr, (size_t)size);
};

void VertexBufferVk::CopyBufferGPUToGPU(VkBuffer &srcBuffer,
                                        VkBuffer &dstBuffer,
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

void VertexBufferVk::UnmapVkMemoryFromCPU(
    VkDeviceMemory &vkVertexBufferMemory) {
  auto &device = DeviceVk::Get()->GetDevice();
  vkUnmapMemory(device, vkVertexBufferMemory);
};

void VertexBufferVk::DestroyVkBuffer(VkBuffer &vkVertexBuffer,
                                     VkDeviceMemory &vkVertexBufferMemory) {
  auto &device = DeviceVk::Get()->GetDevice();
  vkDestroyBuffer(device, vkVertexBuffer, nullptr);
  vkFreeMemory(device, vkVertexBufferMemory, nullptr);
};

void VertexBufferVk::GetDefaultVertexInputBindingDescription(
    VkVertexInputBindingDescription &bindingDescription, const Layout &layout) {
  bindingDescription.binding = 0;
  bindingDescription.stride = layout.strideValue;
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
};

void VertexBufferVk::GetDefaultVertexInputAttributeDescriptions(
    std::vector<VkVertexInputAttributeDescription> &attrDescriptions,
    const Layout &layout) {
  attrDescriptions.resize(layout.countValue);

  for (int i = 0; i < layout.countValue; ++i) {
    attrDescriptions[i].binding = 0;
    attrDescriptions[i].location = i;
    attrDescriptions[i].format =
        MapAttrTypeToVulkanType(layout.attrTypesValue[i]);
    attrDescriptions[i].offset = layout.attrOffsetsValue[i];
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
