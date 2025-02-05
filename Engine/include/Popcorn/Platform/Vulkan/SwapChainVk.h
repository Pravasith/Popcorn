#pragma once

#include "CommonVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class SwapChainVk {
public:
  SwapChainVk() { PC_PRINT("CREATED", TagType::Constr, "SwapChain.h"); };
  ~SwapChainVk() { PC_PRINT("DESTROYED", TagType::Destr, "SwapChain.h"); };

  void CreateSwapChain(const VkDevice &, const SwapChainSupportDetails &,
                       GLFWwindow *, const VkSurfaceKHR &,
                       const QueueFamilyIndices &);
  void CleanUp(const VkDevice &);

private:
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                              GLFWwindow *window);

private:
  VkSwapchainKHR m_swapChain;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
