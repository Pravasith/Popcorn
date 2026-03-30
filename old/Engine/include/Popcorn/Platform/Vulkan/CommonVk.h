#pragma once

#include "GlobalMacros.h"
#include <cstdint>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct SwapchainSupportDetails {
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

constexpr static uint32_t MAX_FRAMES_IN_FLIGHT = 2;

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
