#include "PhysDeviceVk.h"
#include "Global_Macros.h"
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN

void PhysDeviceVk::PickPhysDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_vkInst, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("FAILED TO FIND GPUS WITH VULKAN SUPPORT!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(m_vkInst, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (IsDeviceSuitable(device)) {
      m_physDevice = device;
      break;
    }
  }

  if (m_physDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("FAILED TO FIND A SUITABLE GPU!");
  }
};

bool PhysDeviceVk::IsDeviceSuitable(const VkPhysicalDevice &device) {
  /** FOR CUSTOM IMPLEMENTATION CHECK VULKAN TUTORIAL
   * https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
   */
  FindQueueFamilies(device);
  bool areExtsSupported = CheckDevExtSupport();

  return m_qFamIndices.isComplete() && areExtsSupported;
};

const bool
PhysDeviceVk::CheckDevExtSupport(const VkPhysicalDevice &device) const {
  return true;
};

PhysDeviceVk::QueueFamilyIndices const &PhysDeviceVk::GetQueueFamilyIndices() {
  return m_qFamIndices;
};

void PhysDeviceVk::FindQueueFamilies(const VkPhysicalDevice &device) {
  // LOGIC TO FIND QUEUE FAMILY INDICES TO POPULATE STRUCT WITH

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      m_qFamIndices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

    if (presentSupport) {
      m_qFamIndices.presentFamily = i;
    }

    if (m_qFamIndices.isComplete()) {
      break;
    }

    i++;
  }
}

ENGINE_NAMESPACE_END
