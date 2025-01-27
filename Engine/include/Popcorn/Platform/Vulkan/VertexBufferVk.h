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
  VertexBufferVk(const VertexBufferVk &other) = default;
  VertexBufferVk &operator=(const VertexBufferVk &other) = default;

  // MOVE CONSTRUCTOR
  VertexBufferVk(VertexBufferVk &&other) = default;
  VertexBufferVk &operator=(VertexBufferVk &&other) = default;

  template <typename T>
  static VkVertexInputBindingDescription GetBindingDescription();
  template <typename T>
  static std::array<VkVertexInputAttributeDescription, 2> GetAttrDescriptions();

private:
  virtual uint64_t GetSize() const override { return 0; };
  virtual uint64_t GetCount() const override { return 0; };
  virtual void Bind() override;
  virtual void UnBind() override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
