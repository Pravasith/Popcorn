#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class SwapChainVk {
  friend class RendererVk;

public:
  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  SwapChainSupportDetails QuerySwapChainSupport() const;

private:
  SwapChainVk(const VkPhysicalDevice &physDev, const VkSurfaceKHR &surface)
      : m_physDev(physDev), m_surface(surface) {
    PC_PRINT("CREATED", TagType::Constr, "SWAP-CHAIN-VK");
  }
  ~SwapChainVk() { PC_PRINT("DESTROYED", TagType::Destr, "SWAP-CHAIN-VK"); }

  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

private:
  const VkPhysicalDevice &m_physDev;
  const VkSurfaceKHR &m_surface;
};
ENGINE_NAMESPACE_END
