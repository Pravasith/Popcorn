#pragma once

#include "Popcorn/Core/Base.h"
#include <string>
#include <vector>
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

  [[nodiscard]] inline const VkDevice &GetDevice() const {
    PC_VK_NULL_CHECK(m_device)
    return m_device;
  };

  [[nodiscard]] inline SwapchainSupportDetails
  GetSwapchainSupportDetails(const VkSurfaceKHR &surface) const {
    return QuerySwapchainSupport(m_physicalDevice, surface);
  };

  [[nodiscard]] inline QueueFamilyIndices
  GetQueueFamilyIndices(const VkSurfaceKHR &surface) const {
    return FindQueueFamilies(m_physicalDevice, surface);
  };

  void CreateInstance(const specs &appSpecs);
  void SetupDebugMessenger();
  void PickPhysicalDevice(const VkSurfaceKHR &surface);
  void CreateLogicalDevice(const VkSurfaceKHR &surface);

  void CleanUp();

private:
  std::vector<const char *> GetRequiredExtensions();
  bool CheckDeviceExtensionSupport(const VkPhysicalDevice &);

  /** -------------------------------------------------------------------
   *  ------ VALIDATION LAYERS HELPERS ----------------------------------
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
   *  ------ PHYSICAL DEVICE HELPERS ------------------------------------
   *  ------------------------------------------------------------------- */
  bool IsDeviceSuitable(const VkPhysicalDevice &device,
                        const VkSurfaceKHR &surface);

  QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &device,
                                       const VkSurfaceKHR &surface) const;

  /** -------------------------------------------------------------------
   *  ------ SWAP CHAIN HELPERS -----------------------------------------
   *  ------------------------------------------------------------------- */
  SwapchainSupportDetails
  QuerySwapchainSupport(const VkPhysicalDevice &device,
                        const VkSurfaceKHR &surface) const;

private:
  VkInstance m_instance = VK_NULL_HANDLE;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkDevice m_device = VK_NULL_HANDLE;

  VkQueue m_gfxQueue = VK_NULL_HANDLE;
  VkQueue m_presentQueue = VK_NULL_HANDLE;

  const std::vector<const char *> m_validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  const std::vector<const char *> m_deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

#ifdef NDEBUG
  static constexpr bool s_enableValidationLayers = false;
#else
  static constexpr bool s_enableValidationLayers = true;
#endif
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
