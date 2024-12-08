#pragma once

#include "CommonVk.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class SwapChainVk {
  friend class RendererVk;

public:
  SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice &,
                                                const VkSurfaceKHR &) const;

  [[nodiscard]] inline const VkExtent2D &GetSwapChainExtent() const {
    if (m_swapChainExtent.width == 0 && m_swapChainExtent.height == 0) {
      throw std::runtime_error(
          "SWAP CHAIN EXT IS UNINITIALIZED - SwapChainVk.h");
    }

    return m_swapChainExtent;
  };

  [[nodiscard]] inline const VkFormat &GetImgFormat() const {
    if (m_swapChainImgFormat == VK_FORMAT_UNDEFINED) {
      throw std::runtime_error("SWAPCHAIN IMG FORMAT UNDEFINED");
    };

    return m_swapChainImgFormat;
  };

  [[nodiscard]] inline const std::vector<VkImageView> &GetImgViews() const {
    if (m_swapChainImgViews.size() == 0) {
      throw std::runtime_error("SWAPCHAIN IMG VIEWS NOT INITIALIZED");
    };

    return m_swapChainImgViews;
  };

  [[nodiscard]] inline const VkSwapchainKHR &GetSwapChain() const {
    if (m_swapChain == nullptr) {
      throw std::runtime_error("SWAPCHAIN SWAPCHAIN NOT PRESENT");
    };

    return m_swapChain;
  };

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

  void CreateImgViews(const VkDevice &logiDevice);
  void CleanUp(const VkDevice &logiDevice);

private:
  VkSwapchainKHR m_swapChain;
  std::vector<VkImage> m_swapChainImgs;
  VkFormat m_swapChainImgFormat;
  VkExtent2D m_swapChainExtent{0, 0};
  std::vector<VkImageView> m_swapChainImgViews;
};

ENGINE_NAMESPACE_END
