#include "VkValidationLayers.h"
#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN

auto m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

bool VkValidationLayers::CheckVkVLSupport() {
  uint32_t layerCount = 0;
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
ENGINE_NAMESPACE_END
