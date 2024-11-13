#pragma once

#include "Global_Macros.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN

class VkPhysDevice {
  VkPhysicalDevice physicalDevice;
};

ENGINE_NAMESPACE_END
