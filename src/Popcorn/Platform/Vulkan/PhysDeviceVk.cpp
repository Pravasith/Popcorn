#include "PhysDeviceVk.h"
#include "Global_Macros.h"
#include <vector>

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

bool PhysDeviceVk::IsDeviceSuitable(const VkPhysicalDevice &device) const {
  /** FOR CUSTOM IMPLEMENTATION CHECK VULKAN TUTORIAL
   * https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
   */
  QueueFamilyIndices indices = FindQueueFamilies(device);

  return indices.isComplete();
};

PhysDeviceVk::QueueFamilyIndices
PhysDeviceVk::FindQueueFamilies(VkPhysicalDevice device) const {
  QueueFamilyIndices indices;
  // LOGIC TO FIND QUEUE FAMILY INDICES TO POPULATE STRUCT WITH

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }

    i++;
  }

  return indices;
}

ENGINE_NAMESPACE_END
