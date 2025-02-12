#pragma once

#include "CommonVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class SwapchainVk {
public:
  SwapchainVk() { PC_PRINT("CREATED", TagType::Constr, "Swapchain.h"); };
  ~SwapchainVk() { PC_PRINT("DESTROYED", TagType::Destr, "Swapchain.h"); };

  void CreateSwapchain(const VkDevice &, const SwapchainSupportDetails &,
                       GLFWwindow *, const VkSurfaceKHR &,
                       const QueueFamilyIndices &);

  void CreateImageViews(const VkDevice &);

  [[nodiscard]] inline const VkExtent2D &GetSwapchainExtent() const {
    return m_swapchainExtent;
  };

  [[nodiscard]] inline const VkFormat &GetSwapchainImageFormat() const {
    return m_swapchainImageFormat;
  };

  void CleanUp(const VkDevice &);

private:
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                              GLFWwindow *window);

private:
  VkSwapchainKHR m_swapchain;
  std::vector<VkImage> m_swapchainImages;
  VkFormat m_swapchainImageFormat;
  VkExtent2D m_swapchainExtent;

  std::vector<VkImageView> m_swapchainImageViews;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
