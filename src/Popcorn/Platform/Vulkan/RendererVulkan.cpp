#include <cstdint>
#include <cstring>
#include <vector>

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include "RendererVulkan.h"

// HELLO TRIANGLE!
ENGINE_NAMESPACE_BEGIN

RendererVulkan::RendererVulkan() {
  PC_PRINT_DEBUG("VULKAN CREATED", 2, "RENDERER");
  RendererVulkan::m_enableValidationLayers = false;

  InitVulkan();
  CheckValidationLayerSupport();
};

RendererVulkan::~RendererVulkan() {
  CleanUp();
  PC_PRINT_DEBUG("VULKAN DESTROYED", 2, "RENDERER");
};

void RendererVulkan::InitVulkan() {
  PC_PRINT_DEBUG(s_osWindow, 2, "RENDERER VULKAN")
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

  // CHECK FOR VALIDATION LAYER SUPPORT
  m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
  m_enableValidationLayers = false;
  PC_PRINT_DEBUG("NDEBUG", 0, "VK_R")
#else
  m_enableValidationLayers = true;
  PC_PRINT_DEBUG("DEBUG", 0, "VK_R")
#endif

  if (m_enableValidationLayers && !CheckValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  // CREATE INSTANCE
  VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vkInstance);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("FAILED TO CREATE VK INSTANCE!");
  }
};

bool RendererVulkan::CheckValidationLayerSupport() {

  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : m_validationLayers) {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
};

void RendererVulkan::OnUpdate() const {};

void RendererVulkan::CleanUp() { vkDestroyInstance(m_vkInstance, nullptr); };

ENGINE_NAMESPACE_END
