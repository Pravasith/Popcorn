#include "VertexBufferVk.h"
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

void VertexBufferVk::AllocateVkBuffer(VkBuffer &vkVertexBuffer,
                                      VkDeviceMemory &vkVertexBufferMemory,
                                      VkDeviceSize totalSize) {
  auto *deviceVkStn = DeviceVk::Get();
  auto &device = deviceVkStn->GetDevice();

  VkBufferCreateInfo bufferInfo{};

  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = totalSize;
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

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
      memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &vkVertexBufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error(
        "Failed to allocate GPU memory for the vertex buffer!");
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

void VertexBufferVk::CopyToVkMemory(VkDeviceMemory &vkVertexBufferMemory,
                                    byte_t *destPtr, byte_t *srcPtr,
                                    VkDeviceSize size) {
  auto &device = DeviceVk::Get()->GetDevice();
  memcpy(destPtr, srcPtr, (size_t)size);
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
