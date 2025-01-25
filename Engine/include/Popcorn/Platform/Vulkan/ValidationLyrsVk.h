#pragma once

#include "GlobalMacros.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstring>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
class RendererVk;

class ValidationLyrsVk {
public:
  friend class RendererVk;

private:
  ValidationLyrsVk(const VkInstance &);
  ~ValidationLyrsVk();

  inline const std::vector<const char *> &GetValidationLayers() const {
    return m_validationLayers;
  };

  bool CheckVkVLSupport();
  void
  PopulateDbgMsngrCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  // DEBUG CALLBACK
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

  void SetupDbgMsngr();
  void CleanUp();

private:
  // MEMBERS
  const std::vector<const char *> m_validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  VkDebugUtilsMessengerEXT m_debugMessenger;
  const VkInstance &m_vkInst;
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
