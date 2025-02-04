#pragma once

#include "Popcorn/Core/Base.h"
#include <string>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include "GlobalMacros.h"
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class DeviceVk {
  struct specs {
    /** Instance specs ------ */
    std::string appName;
    int appVersionMajor;
    int appVersionMinor;
    int appVersionPatch;

    /** Physical device specs ------ */
  };

public:
  DeviceVk() { PC_PRINT("CREATED", TagType::Constr, "DeviceVk.h") };
  ~DeviceVk() { PC_PRINT("DESTROYED", TagType::Destr, "DeviceVk.h") };

  void CreateInstance(const specs &appSpecs);
  void SetupDebugMessenger();
  void CleanUp();

private:
  bool CheckValidationLayerSupport();
  std::vector<const char *> GetRequiredExtensions();

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

  void PopulateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

private:
  VkInstance m_instance;
  VkPhysicalDevice m_physicalDevice;
  VkDevice m_device;

  uint32_t m_queueFamilyIndex;
  VkQueue m_queue;

  const std::vector<const char *> m_validationLayers = {
      "VK_LAYER_KHRONOS_validation"};
  VkDebugUtilsMessengerEXT m_debugMessenger;

#ifdef NDEBUG
  static constexpr bool s_enableValidationLayers = false;
#else
  static constexpr bool s_enableValidationLayers = true;
#endif
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
