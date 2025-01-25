#include "VertexBufferVk.h"
#include "GlobalMacros.h"
#include <cstddef>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void VertexBufferVk::Bind() {

};

void VertexBufferVk::UnBind() {

};

template <typename T>
VkVertexInputBindingDescription VertexBufferVk::GetBindingDescription() {
  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(T);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
};

template <typename T>
std::array<VkVertexInputAttributeDescription, 2>
VertexBufferVk::GetAttrDescriptions() {

  std::array<VkVertexInputAttributeDescription, 2> attrDescriptions{};

  // for 'pos'
  attrDescriptions[0].binding = 0;
  attrDescriptions[0].location = 0;
  attrDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attrDescriptions[0].offset = offsetof(T, pos); // 'pos' should be defined in T

  // for 'color'
  attrDescriptions[1].binding = 0;
  attrDescriptions[1].location = 1;
  attrDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attrDescriptions[1].offset =
      offsetof(T, color); // 'color' should be defined in T

  return attrDescriptions;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
