#pragma once

#include "GlobalMacros.h"
#include <cstdint>
#include <optional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  [[nodiscard]] inline bool isComplete() const {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

constexpr static int MAX_FRAMES_IN_FLIGHT = 2;

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
