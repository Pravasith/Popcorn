#pragma once

#include "Popcorn/Core/Base.h"
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include "CommonVk.h"
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
  };

public:
  DeviceVk() { PC_PRINT("CREATED", TagType::Constr, "DeviceVk.h") };
  ~DeviceVk() { PC_PRINT("DESTROYED", TagType::Destr, "DeviceVk.h") };

  [[nodiscard]] inline const VkInstance &GetInstance() const {
    PC_VK_NULL_CHECK(m_instance)
    return m_instance;
  };

  void CreateInstance(const specs &appSpecs);
  void SetupDebugMessenger();
  void PickPhysicalDevice();
  void CreateLogicalDevice();

  void CleanUp();

private:
  std::vector<const char *> GetRequiredExtensions();

  /** -------------------------------------------------------------------
   *  ------ VALIDATION LAYERS ------------------------------------------
   *  ------------------------------------------------------------------- */
  bool CheckValidationLayerSupport();

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

  void PopulateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  /** -------------------------------------------------------------------
   *  ------ PHYSICAL DEVICE --------------------------------------------
   *  ------------------------------------------------------------------- */
  bool IsDeviceSuitable(VkPhysicalDevice device);

  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

  /** -------------------------------------------------------------------
   *  ------ PHYSICAL DEVICE --------------------------------------------
   *  ------------------------------------------------------------------- */

private:
  VkInstance m_instance = VK_NULL_HANDLE;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkDevice m_device = VK_NULL_HANDLE;

  VkQueue m_gfxQueue = VK_NULL_HANDLE;

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
