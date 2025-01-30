#pragma once

#include "CommonVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "SwapChainVk.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
class RendererVk;

// PHYSICAL DEVICE
class PhysDeviceVk {
  friend class RendererVk;
  friend class LogiDeviceVk;

public:
  uint32_t FindGPUMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags) const;

private:
  PhysDeviceVk(const VkInstance &vkInst, const VkSurfaceKHR &surface)
      : m_vkInst(vkInst), m_physDevice(VK_NULL_HANDLE), m_surface(surface) {
    PC_PRINT("CREATED", TagType::Constr, "VK-PHYS-DEVICE");
  };
  ~PhysDeviceVk() { PC_PRINT("DESTROYED", TagType::Destr, "VK-PHYS-DEVICE"); };

  [[nodiscard]] inline const std::vector<const char *> &GetDeviceExts() {
    return m_deviceExts;
  };

  [[nodiscard]] inline const VkPhysicalDevice &GetPhysDevice() const {
    return m_physDevice;
  };

  [[nodiscard]] bool IsDeviceSuitable(const VkPhysicalDevice &,
                                      const SwapChainVk &);

  [[nodiscard]] const bool
  CheckDevExtSupport(const VkPhysicalDevice &device) const;

  const QueueFamilyIndices &GetQueueFamilyIndices();

  void FindQueueFamilies(const VkPhysicalDevice &);
  void PickPhysDevice(const SwapChainVk &);

private:
  const VkInstance &m_vkInst;
  const VkSurfaceKHR &m_surface;

  const std::vector<const char *> m_deviceExts = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkPhysicalDevice m_physDevice;
  QueueFamilyIndices m_qFamIndices;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
