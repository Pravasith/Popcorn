#include "LogiDeviceVk.h"
#include "Global_Macros.h"
#include "PhysDeviceVk.h"
#include "RendererVk.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
void LogiDeviceVk::CreateLogicalDevice(
    const RendererVk::QueueFamilyIndicesVk &qFamIndices,
    const std::vector<const char *> &valLyrsVk,
    const VkPhysicalDevice &physDevVk) {
  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = qFamIndices.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;

  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = 0;

  if constexpr (RendererVk::IsValLyrsEnabled()) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(valLyrsVk.size());
    createInfo.ppEnabledLayerNames = valLyrsVk.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physDevVk, &createInfo, nullptr, &m_device) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }
};

void LogiDeviceVk::CleanUp() { vkDestroyDevice(m_device, nullptr); }

ENGINE_NAMESPACE_END
