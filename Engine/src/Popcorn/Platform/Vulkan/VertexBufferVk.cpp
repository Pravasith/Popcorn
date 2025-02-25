#include "VertexBufferVk.h"
#include "GlobalMacros.h"
#include <stdexcept>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void VertexBufferVk::Bind() {

};

void VertexBufferVk::UnBind() {

};

// void VertexBufferVk::CreateVulkanBuffer(const VkDevice &device,
//                                         const PhysDeviceVk &physDeviceVk) {
//   VkBufferCreateInfo bfrInfo{};
//
//   bfrInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//   bfrInfo.size = GetSize();
//   bfrInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
//   bfrInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//   if (vkCreateBuffer(device, &bfrInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
//   {
//     std::runtime_error("Error creating Vertex Buffer!");
//   };
//
//   //
//   ---------------------------------------------------------------------------
//   // QUERY REQUIREMENTS, AND ALLOCATE VRAM MEMORY
//   ------------------------------
//   //
//   ---------------------------------------------------------------------------
//
//   // QUERY MEMORY REQIREMENTS USING device AND vertexBuffer
//   VkMemoryRequirements memRequirements;
//   vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);
//
//   // ALLOCATE MEMORY
//   VkMemoryAllocateInfo allocInfo{};
//   allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//   allocInfo.allocationSize = memRequirements.size;
//   allocInfo.memoryTypeIndex = physDeviceVk.FindGPUMemoryType(
//       memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//                                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//   if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) !=
//       VK_SUCCESS) {
//     throw std::runtime_error(
//         "Failed to allocate GPU memory for the vertex buffer!");
//   };
//
//   // BIND MEMORY TO THE BUFFER
//   vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
//
//   // FILL VERTEX BUFFER
//   void *data;
//   vkMapMemory(device, vertexBufferMemory, 0, bfrInfo.size, 0, &data);
//   memcpy(data, m_buffer.GetData(), (size_t)bfrInfo.size);
//   vkUnmapMemory(device, vertexBufferMemory);
//
//   // BIND VERTEX BUFFER
// };

// void VertexBufferVk::DestroyVulkanBuffer(const VkDevice &device) {
//   vkDestroyBuffer(device, vertexBuffer, nullptr);
//   vkFreeMemory(device, vertexBufferMemory, nullptr);
// };

VkVertexInputBindingDescription
VertexBufferVk::GetBindingDescription(const VertexBuffer::Layout &layout) {
  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride = layout.strideValue;
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
};

std::vector<VkVertexInputAttributeDescription>
VertexBufferVk::GetAttrDescriptions(const VertexBuffer::Layout &layout) {
  std::vector<VkVertexInputAttributeDescription> attrDescriptions;
  attrDescriptions.resize(layout.countValue);

  for (int i = 0; i < layout.countValue; ++i) {
    attrDescriptions[i].binding = 0;
    attrDescriptions[i].location = i;
    attrDescriptions[i].format =
        MapAttrTypeToVulkanType(layout.attrTypesValue[i]);
    attrDescriptions[i].offset = layout.attrOffsetsValue[i];
  };

  return attrDescriptions;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
