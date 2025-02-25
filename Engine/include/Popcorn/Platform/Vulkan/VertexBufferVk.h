#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "VertexBuffer.h"
#include <cstdint>
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

  // void CreateVulkanBuffer(const VkDevice &, const PhysDeviceVk &);

  // [[nodiscard]] inline const VkBuffer &GetVulkanVertexBuffer() const {
  //   return vertexBuffer;
  // };

  // void DestroyVulkanBuffer(const VkDevice &device);

  // COPY CONSTRUCTOR
  VertexBufferVk(const VertexBufferVk &other) = default;
  VertexBufferVk &operator=(const VertexBufferVk &other) = default;

  // MOVE CONSTRUCTOR
  VertexBufferVk(VertexBufferVk &&other) = default;
  VertexBufferVk &operator=(VertexBufferVk &&other) = default;

  [[nodiscard]] static VkVertexInputBindingDescription
  GetBindingDescription(const Layout &);
  [[nodiscard]] static std::vector<VkVertexInputAttributeDescription>
  GetAttrDescriptions(const Layout &);

  virtual uint64_t GetSize() const override { return m_buffer.GetSize(); };
  virtual uint64_t GetCount() const override { return m_buffer.GetCount(); };

  virtual void Bind() override;
  virtual void UnBind() override;

private:
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
