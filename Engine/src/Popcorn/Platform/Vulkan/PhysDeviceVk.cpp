#include "PhysDeviceVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "SwapChainVk.h"
#include <set>
#include <string>
#include <vector>

ENGINE_NAMESPACE_BEGIN

void PhysDeviceVk::PickPhysDevice(const SwapChainVk &swpChnVk) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_vkInst, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("FAILED TO FIND GPUS WITH VULKAN SUPPORT!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(m_vkInst, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (IsDeviceSuitable(device, swpChnVk)) {
      m_physDevice = device;
      break;
    }
  }

  if (m_physDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("FAILED TO FIND A SUITABLE GPU!");
  }
};

bool PhysDeviceVk::IsDeviceSuitable(const VkPhysicalDevice &device,
                                    const SwapChainVk &swapChainVk) {
  /** FOR CUSTOM IMPLEMENTATION CHECK VULKAN TUTORIAL
   * https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
   */
  FindQueueFamilies(device);
  bool extsSupported = CheckDevExtSupport(device);

  bool swapChainAdequate = false;
  if (extsSupported) {
    SwapChainSupportDetails swapChainSupport =
        swapChainVk.QuerySwapChainSupport(device, m_surface);

    swapChainAdequate = !swapChainSupport.formats.empty() &&
                        !swapChainSupport.presentModes.empty();
  }

  // PC_PRINT("" << (swapChainAdequate ? "True" : "False"), TagType::Print,
  //          "SWAPCHAIN ADEQUATE")

  return m_qFamIndices.isComplete() && extsSupported && swapChainAdequate;
};

// CHECK SWAP CHAIN SUPPORT
const bool
PhysDeviceVk::CheckDevExtSupport(const VkPhysicalDevice &device) const {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,

                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions(m_deviceExts.begin(),
                                           m_deviceExts.end());

  for (const auto &extension : availableExtensions) {
    // PC_PRINT(extension.extensionName, TagType::Print, "CHECK")
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
};

QueueFamilyIndices const &PhysDeviceVk::GetQueueFamilyIndices() {
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
