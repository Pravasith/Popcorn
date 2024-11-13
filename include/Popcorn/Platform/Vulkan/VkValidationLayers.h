#pragma once

#include "Global_Macros.h"
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstring>
#include <vector>

class RendererVulkan;

ENGINE_NAMESPACE_BEGIN
class VkValidationLayers {
public:
  friend class RendererVulkan;

private:
  VkValidationLayers(VkInstance &);
  ~VkValidationLayers();

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
  // CLASS MEMBERS
  VkDebugUtilsMessengerEXT m_DebugMessenger;
  VkInstance &m_vkInst;
};
ENGINE_NAMESPACE_END
