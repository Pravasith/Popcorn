#include <cstdint>
#include <cstring>
#include <vector>

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include "RendererVk.h"

// HELLO TRIANGLE!
ENGINE_NAMESPACE_BEGIN

RendererVulkan::RendererVulkan() : m_VkValLyrs(m_vkInstance) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER-VULKAN");

  InitVulkan();
};

RendererVulkan::~RendererVulkan() {
  CleanUp();
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-VULKAN");
};

void RendererVulkan::InitVulkan() {
  CreateInstance();

  if constexpr (s_enableValidationLayers)
    m_VkValLyrs.SetupDbgMsngr();

  // PickPhysDevice();
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

  auto extensions = GetRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  createInfo.enabledLayerCount = 0;

  VkDebugUtilsMessengerCreateInfoEXT dbgCreateInfo{};

  // CHECK FOR VALIDATION LAYER SUPPORT
  if constexpr (s_enableValidationLayers) {
    if (!m_VkValLyrs.CheckVkVLSupport()) {
      throw std::runtime_error(
          "VALIDATION LAYERS REQUESTED, BUT NOT AVAILABLE!");
    }

    // UPDATE CREATE INFO
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(m_VkValLyrs.GetValidationLayers().size());
    createInfo.ppEnabledLayerNames = m_VkValLyrs.GetValidationLayers().data();

    m_VkValLyrs.PopulateDbgMsngrCreateInfo(dbgCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&dbgCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }

  // CREATE INSTANCE
  VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vkInstance);

  if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS) {
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

void RendererVulkan::CleanUp() {
  m_VkValLyrs.CleanUp();

  vkDestroyInstance(m_vkInstance, nullptr);
  m_vkInstance = VK_NULL_HANDLE;
};

ENGINE_NAMESPACE_END
