#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include "RendererVulkan.h"

// HELLO TRIANGLE!
ENGINE_NAMESPACE_BEGIN

RendererVulkan::RendererVulkan() {
  PC_PRINT_DEBUG("VULKAN CREATED", 2, "RENDERER");

  InitVulkan();
};

RendererVulkan::~RendererVulkan() {
  PC_PRINT_DEBUG("VULKAN DESTROYED", 2, "RENDERER");
  CleanUp();
};

void RendererVulkan::InitVulkan() {
  PC_PRINT_DEBUG(s_os_window, 2, "RENDERER VULKAN")
  CreateInstance();
};

void RendererVulkan::CreateInstance() {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "HELLO TRIANGLE";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Popcorn Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  // EXTENSIONS INFO
  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;
  createInfo.enabledLayerCount = 0;

  // CREATE INSTANCE
  VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vk_instance);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("FAILED TO CREATE VK INSTANCE!");
  }
};

void RendererVulkan::OnUpdate() const {};

void RendererVulkan::CleanUp() { vkDestroyInstance(m_vk_instance, nullptr); };

ENGINE_NAMESPACE_END
