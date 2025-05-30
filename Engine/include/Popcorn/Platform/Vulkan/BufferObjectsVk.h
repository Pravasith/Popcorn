#pragma once

#include "BufferObjects.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class VertexBufferVk : public VertexBuffer {
public:
  // clang-format off
  static constexpr VkFormat MapAttrTypeToVulkanType(BufferDefs::AttrTypes attrType) {
    switch (attrType) {
      case BufferDefs::AttrTypes::Float:  return VK_FORMAT_R32_SFLOAT;
      case BufferDefs::AttrTypes::Float2: return VK_FORMAT_R32G32_SFLOAT;
      case BufferDefs::AttrTypes::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
      case BufferDefs::AttrTypes::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
      case BufferDefs::AttrTypes::Mat3:   return VK_FORMAT_R32G32B32_SFLOAT;  // Typically 3xvec3
      case BufferDefs::AttrTypes::Mat4:   return VK_FORMAT_R32G32B32A32_SFLOAT; // Typically 4xvec4
      case BufferDefs::AttrTypes::Int:    return VK_FORMAT_R32_SINT;
      case BufferDefs::AttrTypes::Int2:   return VK_FORMAT_R32G32_SINT;
      case BufferDefs::AttrTypes::Int3:   return VK_FORMAT_R32G32B32_SINT;
      case BufferDefs::AttrTypes::Int4:   return VK_FORMAT_R32G32B32A32_SINT;
      case BufferDefs::AttrTypes::Bool:   return VK_FORMAT_R8_UINT;
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
      const BufferDefs::Layout &layout,
      VkVertexInputBindingDescription &bindingDescription);

  static void GetDefaultVertexInputAttributeDescriptions(
      const BufferDefs::Layout &layout,
      std::vector<VkVertexInputAttributeDescription> &attrDescriptions);

  //
  // COPY CONSTRUCTOR
  VertexBufferVk(const VertexBufferVk &other) = default;
  VertexBufferVk &operator=(const VertexBufferVk &other) = default;

  //
  // MOVE CONSTRUCTOR
  VertexBufferVk(VertexBufferVk &&other) = default;
  VertexBufferVk &operator=(VertexBufferVk &&other) = default;

  virtual uint64_t GetSize() const override { return m_buffer.GetSize(); };
  virtual uint64_t GetCount() const override {
    return m_buffer.GetCount(m_layout.strideValue);
  };
};

//
//
// --- VULKAN BUFFER MEMORY UTILS -------------------------------------------
// --------------------------------------------------------------------------

class BufferVkUtils {
public:
  static void GetDefaultVkBufferState(VkBufferCreateInfo &bufferInfo,
                                      VkDeviceSize bufferSize);
  // static void AllocateVkBuffer(VkBuffer &vkBuffer,
  //                              VkDeviceMemory &vkBufferMemory,
  //                              const VkBufferCreateInfo &bufferInfo,
  //                              const VkMemoryPropertyFlags
  //                              memoryPropertyFlags);
  // static void DestroyVkBuffer(VkBuffer &vkBuffer,
  //                             VkDeviceMemory &vkBufferMemory);

  // static void *MapVkMemoryToCPU(VkDeviceMemory &vkBufferMemory,
  //                               VkDeviceSize beginOffset,
  //                               VkDeviceSize endOffset);
  // static void CopyBufferCPUToGPU(void *destPtr, void *srcPtr,
  //                                VkDeviceSize size);
  static void CopyStagingToMainBuffers(VkBuffer &srcBuffer, VkBuffer &dstBuffer,
                                       VkDeviceSize size);
  // static void UnmapVkMemoryFromCPU(VkDeviceMemory &vkBufferMemory);

  //
  // --- Record commands ---------------------------------------------------
  static void BindVBO(const VkCommandBuffer &commandBuffer,
                      VkBuffer *vkVertexBuffers, VkDeviceSize *offsets,
                      const uint32_t buffersCount);

  template <Is_Uint16_Or_Uint32_t T>
  static void BindIBO(const VkCommandBuffer &commandBuffer,
                      const VkBuffer &vkIndexBuffer, VkDeviceSize offset) {
    constexpr VkIndexType bufferType = std::is_same_v<uint16_t, T>
                                           ? VK_INDEX_TYPE_UINT16
                                           : VK_INDEX_TYPE_UINT32;
    vkCmdBindIndexBuffer(commandBuffer, vkIndexBuffer, offset, bufferType);
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
