#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "VertexBuffer.h"
#include <array>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class VertexBufferVk : public VertexBuffer {
public:
  VertexBufferVk() { PC_PRINT("CREATED", TagType::Constr, "VERTEX-BUFFER-VK") };
  ~VertexBufferVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "VERTEX-BUFFER-VK")
  };

  template <typename T>
  static VkVertexInputBindingDescription GetBindingDescription();
  template <typename T>
  static std::array<VkVertexInputAttributeDescription, 2> GetAttrDescriptions();

private:
  virtual void Bind() override;
  virtual void UnBind() override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
