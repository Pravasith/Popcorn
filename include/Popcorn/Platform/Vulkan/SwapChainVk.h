#pragma once

#include "CommonVk.h"
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

  [[nodiscard]] inline const VkExtent2D &GetSwapChainExtent() const {
    if (m_swapChainExtent.width == 0 && m_swapChainExtent.height == 0) {
      PC_WARN("SWAP CHAIN EXT IS UNINITIALIZED - SwapChainVk.h");
    }

    return m_swapChainExtent;
  };

  [[nodiscard]] inline const VkFormat &GetImgFormat() const {
    if (m_swapChainImgFormat == VK_FORMAT_UNDEFINED) {
      PC_WARN("SWAPCHAIN IMG FORMAT UNDEFINED");
    };

    return m_swapChainImgFormat;
  };

  [[nodiscard]] inline const std::vector<VkImageView> &GetImgViews() const {
    if (m_swapChainImgViews.size() == 0) {
      PC_WARN("SWAPCHAIN IMG VIEWS NOT INITIALIZED");
    };

    return m_swapChainImgViews;
  };

  [[nodiscard]] inline const std::vector<VkFramebuffer> &GetFrameBfrs() const {
    if (m_frameBfrs.size() == 0) {
      PC_WARN("ERROR: SWAP CHAIN FRAME BUFFERS VEC IS EMPTY!");
    };

    return m_frameBfrs;
  };

  [[nodiscard]] inline const VkSwapchainKHR &GetSwapChain() const {
    if (m_swapChain == nullptr) {
      PC_WARN("SWAPCHAIN NOT PRESENT");
    };

    return m_swapChain;
  };

private:
  SwapChainVk(const VkPhysicalDevice &physDev, const VkSurfaceKHR &surface) {
    PC_PRINT("CREATED", TagType::Constr, "SWAP-CHAIN-VK");
  }
  ~SwapChainVk() { PC_PRINT("DESTROYED", TagType::Destr, "SWAP-CHAIN-VK"); }

  void CreateSwapChain(const VkPhysicalDevice &physDev,
                       const VkSurfaceKHR &surface,
                       const QueueFamilyIndices &qFamIndices,
                       const VkDevice &logiDevice, const uint32_t frmBfrWidth,
                       const uint32_t frmBfrHeight);
  void CreateImgViews(const VkDevice &logiDevice);
  void CreateFrameBfrs(const VkDevice &dev, const VkRenderPass &rndrPass);
  void RecreateSwapChain(const VkDevice &logiDevice,
                         const VkPhysicalDevice &physDev,
                         const VkSurfaceKHR &srfc, GLFWwindow *osWindow,
                         const QueueFamilyIndices &qFamIndices,
                         const VkRenderPass &rndrPass,
                         const uint32_t frmBfrWidth,
                         const uint32_t frmBfrHeight);

  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                              uint32_t frameBfrW, uint32_t frameBfrH);

  void CleanUp(const VkDevice &logiDevice);

private:
  VkSwapchainKHR m_swapChain;
  std::vector<VkImage> m_swapChainImgs;
  VkFormat m_swapChainImgFormat;
  VkExtent2D m_swapChainExtent{0, 0};
  std::vector<VkImageView> m_swapChainImgViews;
  std::vector<VkFramebuffer> m_frameBfrs;
};

ENGINE_NAMESPACE_END
