#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "VertexBuffer.h"
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class VertexBufferVk : public VertexBuffer {
public:
  // clang-format off
  static constexpr VkFormat MapAttrTypeToVulkanType(AttrTypes attrType) {
    switch (attrType) {
      case AttrTypes::Float:  return VK_FORMAT_R32_SFLOAT;
      case AttrTypes::Float2: return VK_FORMAT_R32G32_SFLOAT;
      case AttrTypes::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
      case AttrTypes::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
      case AttrTypes::Mat3:   return VK_FORMAT_R32G32B32_SFLOAT;  // Typically 3xvec3
      case AttrTypes::Mat4:   return VK_FORMAT_R32G32B32A32_SFLOAT; // Typically 4xvec4
      case AttrTypes::Int:    return VK_FORMAT_R32_SINT;
      case AttrTypes::Int2:   return VK_FORMAT_R32G32_SINT;
      case AttrTypes::Int3:   return VK_FORMAT_R32G32B32_SINT;
      case AttrTypes::Int4:   return VK_FORMAT_R32G32B32A32_SINT;
      case AttrTypes::Bool:   return VK_FORMAT_R8_UINT;  
      default:
        PC_ASSERT(false, "Unknown AttrType");
        return VK_FORMAT_R8_UINT;
    }
  };
  // clang-format on

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

  static VkVertexInputBindingDescription GetBindingDescription(const Layout &);
  static std::vector<VkVertexInputAttributeDescription>
  GetAttrDescriptions(const Layout &);

private:
  virtual uint64_t GetSize() const override { return 0; };
  virtual uint64_t GetCount() const override { return 0; };
  virtual void Bind() override;
  virtual void UnBind() override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
