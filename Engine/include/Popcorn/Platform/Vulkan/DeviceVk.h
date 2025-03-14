#pragma once

#include "Popcorn/Core/Base.h"
#include <cstdint>
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
  [[nodiscard]] inline static DeviceVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new DeviceVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of MaterialHandler")
    };
  };

  [[nodiscard]] inline const VkInstance &GetVkInstance() const {
    PC_VK_NULL_CHECK(m_vkInstance)
    return m_vkInstance;
  };

  [[nodiscard]] inline const VkPhysicalDevice &GetPhysicalDevice() const {
    PC_VK_NULL_CHECK(m_physicalDevice)
    return m_physicalDevice;
  };

  [[nodiscard]] inline const VkDevice &GetDevice() const {
    PC_VK_NULL_CHECK(m_device)
    return m_device;
  };

  [[nodiscard]] inline const VkQueue &GetGraphicsQueue() const {
    PC_VK_NULL_CHECK(m_graphicsQueue)
    return m_graphicsQueue;
  };

  [[nodiscard]] inline const VkQueue &GetPresentQueue() const {
    PC_VK_NULL_CHECK(m_presentQueue)
    return m_presentQueue;
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
  uint32_t FindMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

  void CleanUp();

private:
  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  DeviceVk(const DeviceVk &) = delete;
  DeviceVk &operator=(const DeviceVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  DeviceVk(DeviceVk &&) = delete;
  DeviceVk &operator=(DeviceVk &&) = delete;

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

public:
  static constexpr uint32_t VULKAN_API_VERSION = VK_API_VERSION_1_2;

private:
  DeviceVk() { PC_PRINT("CREATED", TagType::Constr, "DeviceVk.h") };
  ~DeviceVk() { PC_PRINT("DESTROYED", TagType::Destr, "DeviceVk.h") };

  static DeviceVk *s_instance;

  VkInstance m_vkInstance = VK_NULL_HANDLE;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkDevice m_device = VK_NULL_HANDLE;

  VkQueue m_graphicsQueue = VK_NULL_HANDLE;
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
