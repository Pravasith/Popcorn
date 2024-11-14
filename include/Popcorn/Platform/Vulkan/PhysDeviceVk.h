#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <optional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class RendererVk;

// PHYSICAL DEVICE
class PhysDeviceVk {
  friend class RendererVk;

private:
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    bool isComplete() { return graphicsFamily.has_value(); }
  };

private:
  PhysDeviceVk(const VkInstance &vkInst)
      : m_vkInst(vkInst), m_physDevice(VK_NULL_HANDLE) {
    PC_PRINT("CREATED", TagType::Constr, "VK-PHYS-DEVICE");
  };
  ~PhysDeviceVk() { PC_PRINT("DESTROYED", TagType::Destr, "VK-PHYS-DEVICE"); };

  [[nodiscard]] bool IsDeviceSuitable(const VkPhysicalDevice &) const;
  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;

  void PickPhysDevice();

private:
  const VkInstance &m_vkInst;
  VkPhysicalDevice m_physDevice;
};

ENGINE_NAMESPACE_END
