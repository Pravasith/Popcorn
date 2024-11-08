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

  m_VkValLyrs = VkValidationLayers();
  InitVulkan();
};

RendererVulkan::~RendererVulkan() {
  CleanUp();
  PC_PRINT_DEBUG("VULKAN DESTROYED", 2, "RENDERER");
};

void RendererVulkan::InitVulkan() {
  PC_PRINT_DEBUG(s_osWindow, 2, "RENDERER VULKAN")
  CreateInstance();
  // CheckValidationLayerSupport();
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
  if constexpr (s_enableValidationLayers) {
    if (!m_VkValLyrs.CheckVkVLSupport()) {
      throw std::runtime_error(
          "VALIDATION LAYERS REQUESTED, BUT NOT AVAILABLE!");

      // UPDATE CREATE INFO
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(m_VkValLyrs.GetValidationLayers().size());
      createInfo.ppEnabledLayerNames = m_VkValLyrs.GetValidationLayers().data();
    }
  } else {
    createInfo.enabledLayerCount = 0;
  }

  auto extensions = GetRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  // CREATE INSTANCE
  VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vkInstance);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("FAILED TO CREATE VK INSTANCE!");
  }
};

std::vector<const char *> RendererVulkan::GetRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if constexpr (s_enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  };

  return extensions;
};

void RendererVulkan::OnUpdate() const {};

void RendererVulkan::CleanUp() { vkDestroyInstance(m_vkInstance, nullptr); };

ENGINE_NAMESPACE_END
