#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <optional>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class RendererVk;

// PHYSICAL DEVICE
class PhysDeviceVk {
  friend class RendererVk;
  friend class LogiDeviceVk;

public:
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

private:
  PhysDeviceVk(const VkInstance &vkInst, const VkSurfaceKHR &surface)
      : m_vkInst(vkInst), m_physDevice(VK_NULL_HANDLE), m_surface(surface) {
    PC_PRINT("CREATED", TagType::Constr, "VK-PHYS-DEVICE");
  };
  ~PhysDeviceVk() { PC_PRINT("DESTROYED", TagType::Destr, "VK-PHYS-DEVICE"); };

  [[nodiscard]] inline const VkPhysicalDevice &GetPhysDevice() const {
    return m_physDevice;
  };

  [[nodiscard]] bool IsDeviceSuitable(const VkPhysicalDevice &);

  const QueueFamilyIndices &GetQueueFamilyIndices();

  void FindQueueFamilies(const VkPhysicalDevice &);
  void PickPhysDevice();

private:
  const VkInstance &m_vkInst;
  const VkSurfaceKHR &m_surface;

  VkPhysicalDevice m_physDevice;
  QueueFamilyIndices m_qFamIndices;
};

ENGINE_NAMESPACE_END
