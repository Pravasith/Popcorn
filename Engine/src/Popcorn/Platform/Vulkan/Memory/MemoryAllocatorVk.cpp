#include "Memory/MemoryAllocatorVk.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

MemoryAllocatorVk *MemoryAllocatorVk::s_instance = nullptr;
DeviceVk *MemoryAllocatorVk::s_deviceVk = nullptr;

void MemoryAllocatorVk::CreateVMAAllocator() {
  VmaAllocatorCreateInfo allocatorCreateInfo = {};
  allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
  allocatorCreateInfo.vulkanApiVersion = DeviceVk::VULKAN_API_VERSION;
  allocatorCreateInfo.physicalDevice = s_deviceVk->GetPhysicalDevice();
  allocatorCreateInfo.device = s_deviceVk->GetDevice();
  allocatorCreateInfo.instance = s_deviceVk->GetVkInstance();

  if (vmaCreateAllocator(&allocatorCreateInfo, &m_allocator) != VK_SUCCESS) {
    std::runtime_error("failed to create vmaAllocator");
  }
};

void MemoryAllocatorVk::AllocateBufferMemory(VmaAllocation &allocation) {};
void MemoryAllocatorVk::AllocateImageMemory(VmaAllocation &allocation) {};

void MemoryAllocatorVk::CleanUp() { vmaDestroyAllocator(m_allocator); };

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
