#pragma once

#include "Common.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class SwapChainVk {
  friend class RendererVk;

public:
  SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice &,
                                                const VkSurfaceKHR &) const;

private:
  SwapChainVk(const VkPhysicalDevice &physDev, const VkSurfaceKHR &surface) {
    PC_PRINT("CREATED", TagType::Constr, "SWAP-CHAIN-VK");
  }
  ~SwapChainVk() { PC_PRINT("DESTROYED", TagType::Destr, "SWAP-CHAIN-VK"); }

  void CreateSwapChain(const VkPhysicalDevice &, const VkSurfaceKHR &,
                       GLFWwindow *, const QueueFamilyIndices &,
                       const VkDevice &);

  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                              GLFWwindow *osWindow);

  void CleanUp(const VkDevice &logiDevice);

private:
  VkSwapchainKHR m_swapChain;
};
ENGINE_NAMESPACE_END
