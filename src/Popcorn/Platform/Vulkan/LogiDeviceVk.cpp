#include "LogiDeviceVk.h"
#include "Global_Macros.h"
#include "PhysDeviceVk.h"
#include "RendererVk.h"
#include <set>

ENGINE_NAMESPACE_BEGIN
void LogiDeviceVk::CreateLogicalDevice(
    const QueueFamilyIndices &qFamIndices,
    const std::vector<const char *> &valLyrsVk,
    const VkPhysicalDevice &physDevVk,
    const std::vector<const char *> &devExts) {

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

  createInfo.enabledExtensionCount = static_cast<uint32_t>(devExts.size());
  createInfo.ppEnabledExtensionNames = devExts.data();

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

  vkGetDeviceQueue(m_device, qFamIndices.graphicsFamily.value(), 0,
                   &m_gfxQueue);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {qFamIndices.graphicsFamily.value(),
                                            qFamIndices.presentFamily.value()};

  queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  vkGetDeviceQueue(m_device, qFamIndices.presentFamily.value(), 0,
                   &m_presQueue);
};

void LogiDeviceVk::CleanUp() { vkDestroyDevice(m_device, nullptr); }

ENGINE_NAMESPACE_END
