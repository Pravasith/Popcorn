#include "VertexBufferVk.h"
#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Buffer.h"
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
    const VkCommandBuffer &commandBuffer) {
  VkBuffer vertexBuffers[] = {m_vkBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
};

void VertexBufferVk::AllocateVkBuffer() {
  auto *deviceVkStn = DeviceVk::Get();
  auto &device = deviceVkStn->GetDevice();

  VkBufferCreateInfo bufferInfo{};

  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = GetSize();
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_vkBuffer) != VK_SUCCESS) {
    std::runtime_error("Error creating Vertex Buffer!");
  };
  //
  // QUERY MEMORY REQIREMENTS USING device AND m_vkBuffer
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, m_vkBuffer, &memRequirements);

  //
  // ALLOCATE MEMORY
  // TODO: Use VMA to allocate memory
  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = deviceVkStn->FindMemoryType(
      memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &m_vertexBufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error(
        "Failed to allocate GPU memory for the vertex buffer!");
  };

  //
  // BIND MEMORY TO THE BUFFER
  vkBindBufferMemory(device, m_vkBuffer, m_vertexBufferMemory, 0);

  //
  // FILL VERTEX BUFFER
  void *data;
  vkMapMemory(device, m_vertexBufferMemory, 0, bufferInfo.size, 0, &data);
  memcpy(data, m_buffer.GetData(), (size_t)bufferInfo.size);
  vkUnmapMemory(device, m_vertexBufferMemory);
};

void VertexBufferVk::DestroyVkBuffer() {
  auto &device = DeviceVk::Get()->GetDevice();
  vkDestroyBuffer(device, m_vkBuffer, nullptr);
  vkFreeMemory(device, m_vertexBufferMemory, nullptr);
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
