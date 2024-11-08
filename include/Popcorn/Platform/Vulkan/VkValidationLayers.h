#pragma once

#include "Global_Macros.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstring>
#include <vector>

ENGINE_NAMESPACE_BEGIN
class VkValidationLayers {
public:
  bool CheckVkVLSupport();
  std::vector<const char *> GetValidationLayers() const {
    return m_validationLayers;
  };

private:
  std::vector<const char *> m_validationLayers;
};
ENGINE_NAMESPACE_END
