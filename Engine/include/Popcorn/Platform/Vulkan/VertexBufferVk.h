#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include "VertexBuffer.h"
#include <cstdint>
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

  //
  // --- UTILS --------------------------------------------------------------
  static void GetDefaultVertexInputBindingDescription(
      VkVertexInputBindingDescription &bindingDescription,
      const Layout &layout);

  static void GetDefaultVertexInputAttributeDescriptions(
      std::vector<VkVertexInputAttributeDescription> &, const Layout &);

  //
  // COPY CONSTRUCTOR
  VertexBufferVk(const VertexBufferVk &other) = default;
  VertexBufferVk &operator=(const VertexBufferVk &other) = default;

  // MOVE CONSTRUCTOR
  VertexBufferVk(VertexBufferVk &&other) = default;
  VertexBufferVk &operator=(VertexBufferVk &&other) = default;

  virtual uint64_t GetSize() const override { return m_buffer.GetSize(); };
  virtual uint64_t GetCount() const override { return m_buffer.GetCount(); };

  virtual void Bind() override;
  virtual void UnBind() override;
};

//
//
// --- VULKAN BUFFER MEMORY UTILS -------------------------------------------
// --------------------------------------------------------------------------

class BufferVkUtils {
public:
  static void GetDefaultVkBufferState(VkBufferCreateInfo &bufferInfo,
                                      VkDeviceSize bufferSize);
  static void AllocateVkBuffer(VkBuffer &vkBuffer,
                               VkDeviceMemory &vkBufferMemory,
                               const VkBufferCreateInfo &bufferInfo,
                               const VkMemoryPropertyFlags memoryPropertyFlags);
  static void DestroyVkBuffer(VkBuffer &vkBuffer,
                              VkDeviceMemory &vkBufferMemory);

  static void *MapVkMemoryToCPU(VkDeviceMemory &vkBufferMemory,
                                VkDeviceSize beginOffset,
                                VkDeviceSize endOffset);
  static void CopyBufferCPUToGPU(VkDeviceMemory &vkBufferMemory,
                                 byte_t *destPtr, byte_t *srcPtr,
                                 VkDeviceSize size);
  static void CopyBufferGPUToGPU(VkBuffer &srcBuffer, VkBuffer &dstBuffer,
                                 VkDeviceSize size);
  static void UnmapVkMemoryFromCPU(VkDeviceMemory &vkBufferMemory);

  //
  // --- Record commands ---------------------------------------------------
  static void RecordBindVkVertexBuffersCommand(
      const VkCommandBuffer &commandBuffer, VkBuffer *vkVertexBuffers,
      VkDeviceSize *offsets, const uint32_t buffersCount);

  template <Is_Uint16_Or_Uint32_t T>
  static void
  RecordBindVkIndexBufferCommand(const VkCommandBuffer &commandBuffer,
                                 VkBuffer *vkIndexBuffer, VkDeviceSize offset) {
    constexpr VkIndexType bufferType = std::is_same_v<uint16_t, T>
                                           ? VK_INDEX_TYPE_UINT16
                                           : VK_INDEX_TYPE_UINT32;
    vkCmdBindIndexBuffer(commandBuffer, *vkIndexBuffer, 0, bufferType);
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
