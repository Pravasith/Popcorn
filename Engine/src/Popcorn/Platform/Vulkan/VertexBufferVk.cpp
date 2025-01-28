#include "VertexBufferVk.h"
#include "GlobalMacros.h"
#include <glm/glm.hpp>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void VertexBufferVk::Bind() {

};

void VertexBufferVk::UnBind() {

};

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
  std::vector<VkVertexInputAttributeDescription> attrDescriptions{};

  for (int i = 0; i < layout.countValue; ++i) {
    attrDescriptions[i].binding = 0;
    attrDescriptions[i].location = i;
    attrDescriptions[i].format =
        MapAttrTypeToVulkanType(layout.attrTypesValue[i]);
    attrDescriptions[i].offset = layout.attrOffsetsValue[i];
  };

  // for 'pos'
  // attrDescriptions[0].binding = 0;
  // attrDescriptions[0].location = 0;
  // attrDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  // attrDescriptions[0].offset = offsetof(T, pos); // 'pos' should be defined
  // in T

  // for 'color'
  // attrDescriptions[1].binding = 0;
  // attrDescriptions[1].location = 1;
  // attrDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  // attrDescriptions[1].offset =
  //     offsetof(T, color); // 'color' should be defined in T

  return attrDescriptions;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
