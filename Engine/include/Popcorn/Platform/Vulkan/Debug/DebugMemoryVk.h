#ifdef PC_DEBUG

#pragma once

#include "GlobalMacros.h"
#include "MemoryVk.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class DebugDeviceMemoryVk {
public:
  DebugDeviceMemoryVk() {
    PC_PRINT("CREATED", TagType::Constr, "PcDebugDeviceMemoryVk.h")
  };
  ~DebugDeviceMemoryVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "PcDebugDeviceMemoryVk.h")
  };

  [[nodiscard]] void *
  CreateStagingBuffer(VkDevice &device, VmaAllocator &vmaAllocator,
                      VkBuffer &srcBuffer, VkDeviceSize size,
                      VkCommandPool &commandPool, VkQueue &queue);
  void DestroyStagingBuffer(VmaAllocator &allocator);
  void CopyBuffer(VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize size,
                  VkDevice &device, VkCommandPool &commandPool, VkQueue &queue);

  void PrintVmaBuffer(VkDeviceSize size);
  static void PrintRawGpuVertexDataFromBytes(const void *rawVboData,
                                             size_t vboSize,
                                             const void *rawIboData,
                                             size_t iboSize);

private:
  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  DebugDeviceMemoryVk(const DebugDeviceMemoryVk &) = delete;
  DebugDeviceMemoryVk &operator=(const DebugDeviceMemoryVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  DebugDeviceMemoryVk(DebugDeviceMemoryVk &&) = delete;
  DebugDeviceMemoryVk &operator=(DebugDeviceMemoryVk &&) = delete;

private:
  void *m_data = nullptr;

  VkBuffer m_stagingBuffer;
  VmaAllocation m_stagingAllocation;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END

#endif
