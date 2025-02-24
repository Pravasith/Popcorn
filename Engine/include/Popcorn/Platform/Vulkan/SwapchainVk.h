#pragma once

#include "CommonVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class SwapchainVk {
public:
  static SwapchainVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new SwapchainVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of SwapchainVk")
    };
  };

  void CreateSwapchain(const VkDevice &, const SwapchainSupportDetails &,
                       GLFWwindow *, const VkSurfaceKHR &,
                       const QueueFamilyIndices &);

  void CreateImageViews(const VkDevice &);

  [[nodiscard]] inline const VkSwapchainKHR &GetVkSwapchain() const {
    return m_swapchain;
  };

  [[nodiscard]] inline const VkExtent2D &GetSwapchainExtent() const {
    return m_swapchainExtent;
  };

  [[nodiscard]] inline const VkFormat &GetSwapchainImageFormat() const {
    return m_swapchainImageFormat;
  };

  [[nodiscard]] inline const std::vector<VkImageView> &
  GetSwapchainImageViews() const {
    return m_swapchainImageViews;
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
  SwapchainVk() { PC_PRINT("CREATED", TagType::Constr, "SwapchainVk.h"); };
  ~SwapchainVk() { PC_PRINT("DESTROYED", TagType::Destr, "SwapchainVk.h"); };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  SwapchainVk(const SwapchainVk &) = delete;
  SwapchainVk &operator=(const SwapchainVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  SwapchainVk(SwapchainVk &&) = delete;
  SwapchainVk &operator=(SwapchainVk &&) = delete;

private:
  static SwapchainVk *s_instance;

  VkSwapchainKHR m_swapchain;
  std::vector<VkImage> m_swapchainImages;
  VkFormat m_swapchainImageFormat;
  VkExtent2D m_swapchainExtent;

  std::vector<VkImageView> m_swapchainImageViews;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
