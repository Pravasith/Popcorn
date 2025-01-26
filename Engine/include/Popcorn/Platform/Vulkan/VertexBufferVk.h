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

  // COPY CONSTRUCTOR
  VertexBufferVk(const VertexBufferVk &other) {
    PC_PRINT("COPY CONSTRUCTOR EVOKED", TagType::Print, "Vertex-Buffer-Vk")
    m_buffer = other.m_buffer;
  };
  VertexBufferVk &operator=(const VertexBufferVk &other) {
    PC_PRINT("COPY ASSIGNMENT EVOKED", TagType::Print, "Vertex-Buffer-Vk")

    if (this == &other)
      return *this;

    m_buffer = other.m_buffer;
    return *this;
  };

  // MOVE CONSTRUCTOR
  VertexBufferVk(VertexBufferVk &&other) {
    PC_PRINT("MOVE CONSTRUCTOR EVOKED", TagType::Print, "Vertex-Buffer-Vk")
    if (this == &other) {
      return;
    };
    m_buffer = other.m_buffer;
  };
  VertexBufferVk &&operator=(VertexBufferVk &&other) {
    PC_PRINT("MOVE ASSIGNMENT EVOKED", TagType::Print, "Vertex-Buffer-Vk")

    if (this == &other) {
      return std::move(*this);
    };

    m_buffer = std::move(other.m_buffer);
    return std::move(*this);
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
