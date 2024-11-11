#pragma once

#include "Global_Macros.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstring>
#include <vector>

ENGINE_NAMESPACE_BEGIN
class VkValidationLayers {
public:
  VkValidationLayers();
  ~VkValidationLayers() = default;

  inline std::vector<const char *> GetValidationLayers() const {
    return m_validationLayers;
  };

  bool CheckVkVLSupport();

  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
      /**
       * VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
       * VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Informational message
  like the creation of a resource
       * VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Message about behavior
  that is not necessarily an error, but very likely a bug in your application
       * VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Message about behavior
  that is invalid and may cause crashes */
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      /**
       * VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: Some event has happened
that is unrelated to the specification or performance
       * VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: Something has happened
that violates the specification or indicates a possible mistake
       * VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: Potential non-optimal
use of Vulkan */
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
      void *pUserData);

  void SetupDbgMsngr();

private:
  // MEMBERS
  const std::vector<const char *> m_validationLayers = {
      "VK_LAYER_KHRONOS_validation"};
  // CLASS MEMBERS
  VkDebugUtilsMessengerEXT m_DebugMessenger;
};
ENGINE_NAMESPACE_END
