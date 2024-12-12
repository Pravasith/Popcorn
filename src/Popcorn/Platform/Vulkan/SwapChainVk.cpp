#include "SwapChainVk.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <algorithm>
#include <cstdint>
#include <limits>

ENGINE_NAMESPACE_BEGIN
SwapChainSupportDetails
SwapChainVk::QuerySwapChainSupport(const VkPhysicalDevice &physDev,
                                   const VkSurfaceKHR &surface) const {

  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDev, surface,
                                            &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physDev, surface, &formatCount, nullptr);
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physDev, surface, &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physDev, surface, &presentModeCount,
                                            nullptr);
  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physDev, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

VkSurfaceFormatKHR SwapChainVk::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {
  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
};

VkPresentModeKHR SwapChainVk::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

void SwapChainVk::RecreateSwapChain(const uint32_t frmBfrWidth,
                                    const uint32_t frmBfrHeight) {
  vkDeviceWaitIdle(m_logiDevice);
  CleanUp();
  CreateSwapChain(frmBfrWidth, frmBfrHeight);
  CreateImgViews();
  CreateFrameBfrs();
};

void SwapChainVk::RecreateSwapChainFtr::operator()(const uint32_t width,
                                                   const uint32_t height) {
  vkDeviceWaitIdle(m_logiDev);
  m_instance.CleanUp();

  m_instance.CreateSwapChain(width, height);
  m_instance.CreateImgViews();
  m_instance.CreateFrameBfrs();
};

VkExtent2D
SwapChainVk::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                              uint32_t frameBfrW, uint32_t frameBfrH) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {

    VkExtent2D actualExtent{frameBfrW, frameBfrH};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
};

void SwapChainVk::CreateSwapChain(const uint32_t frmBfrWidth,
                                  const uint32_t frmBfrHeight) {
  SwapChainSupportDetails swapChainSupport =
      QuerySwapChainSupport(m_physDev, m_srfc);

  VkSurfaceFormatKHR surfaceFormat =
      ChooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      ChooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities,
                                       frmBfrWidth, frmBfrHeight);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = m_srfc;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queueFamilyIndices[] = {m_qFamIndices.graphicsFamily.value(),
                                   m_qFamIndices.presentFamily.value()};

  if (m_qFamIndices.graphicsFamily != m_qFamIndices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;     // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(m_logiDevice, &createInfo, nullptr, &m_swapChain) !=
      VK_SUCCESS) {
    throw std::runtime_error("FAILED TO CREATE SWAP CHAIN!");
  }

  // RETRIEVE HANDLES OF THE SWAPCHAIN IMAGES
  vkGetSwapchainImagesKHR(m_logiDevice, m_swapChain, &imageCount, nullptr);
  m_swapChainImgs.resize(imageCount);
  vkGetSwapchainImagesKHR(m_logiDevice, m_swapChain, &imageCount,
                          m_swapChainImgs.data());

  m_swapChainImgFormat = surfaceFormat.format;
  m_swapChainExtent = extent;
}

void SwapChainVk::CreateFrameBfrs() {
  m_frameBfrs.resize(m_swapChainImgViews.size());

  for (size_t i = 0; i < m_swapChainImgViews.size(); ++i) {
    VkImageView attachments[] = {m_swapChainImgViews[i]};

    VkFramebufferCreateInfo frmbfrInfo{};
    frmbfrInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frmbfrInfo.renderPass = m_rndrPass;
    frmbfrInfo.attachmentCount = 1;
    frmbfrInfo.pAttachments = attachments;
    frmbfrInfo.width = m_swapChainExtent.width;
    frmbfrInfo.height = m_swapChainExtent.height;
    frmbfrInfo.layers = 1;

    if (vkCreateFramebuffer(m_logiDevice, &frmbfrInfo, nullptr,
                            &m_frameBfrs[i]) != VK_SUCCESS) {
      throw std::runtime_error("ERROR: FAILED TO CREATE FRAMEBUFFER!");
    };
  };
};

void SwapChainVk::CreateImgViews() {
  m_swapChainImgViews.resize(m_swapChainImgs.size());

  PC_PRINT(m_swapChainImgViews.size(), TagType::Print, "SC IMAGE VIEWS SIZE")

  for (size_t i = 0; i < m_swapChainImgs.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = m_swapChainImgs[i];

    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = m_swapChainImgFormat;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_logiDevice, &createInfo, nullptr,
                          &m_swapChainImgViews[i]) != VK_SUCCESS) {
      throw std::runtime_error("FAILED TO CREATE IMAGE VIEWS!");
    }
  }
};

void SwapChainVk::CleanUp() const {
  for (size_t i = 0; i < m_frameBfrs.size(); ++i) {
    vkDestroyFramebuffer(m_logiDevice, m_frameBfrs[i], nullptr);
  };
  for (auto imageView : m_swapChainImgViews) {
    vkDestroyImageView(m_logiDevice, imageView, nullptr);
  }
  vkDestroySwapchainKHR(m_logiDevice, m_swapChain, nullptr);
}

ENGINE_NAMESPACE_END
