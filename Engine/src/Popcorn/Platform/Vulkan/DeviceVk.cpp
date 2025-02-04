#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <stdexcept>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void DeviceVk::CreateInstance() { CreateInstance({"Vulkan App", 1, 0, 0}); };
void DeviceVk::CreateInstance(const specs &appSpecs = specs{"Vulkan App", 1, 0,
                                                            0}) {
  if (m_instance) {
    PC_WARN("Instance already created!")
    return;
  };

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = appSpecs.appName.c_str();
  appInfo.apiVersion =
      VK_MAKE_VERSION(appSpecs.appVersionMajor, appSpecs.appVersionMinor,
                      appSpecs.appVersionPatch);
  appInfo.pEngineName = "Popcorn Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;
  createInfo.enabledLayerCount = 0;

  if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }

  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());

  PC_PRINT("Available Extensions: \n", TagType::Print, "DeviceVk.h")

  for (const auto &extension : extensions) {
    PC_PRINT(extension.extensionName << "\t", TagType::Print, "DeviceVk.h")
  }
};

void DeviceVk::CleanUp() { vkDestroyInstance(m_instance, nullptr); };

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
