#include "SwapchainVk.h"
#include "DeviceVk.h"
#include "Popcorn/Core/Base.h"
#include "SurfaceVk.h"
#include <algorithm>
#include <limits>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

SwapchainVk *SwapchainVk::s_instance = nullptr;

void SwapchainVk::CreateSwapchainImagesAndVkSwapchain() {
  if (m_appWin == nullptr) {
    PC_ERROR("m_appWin is nullptr", "SwapchainVk")
  };

  GLFWwindow *window = (GLFWwindow *)m_appWin->GetOSWindow();

  auto *swapchainVkStn = SwapchainVk::Get();
  auto *deviceVkStn = DeviceVk::Get();

  const auto &surface = SurfaceVk::Get()->GetSurface();
  auto &device = deviceVkStn->GetDevice();

  const auto &swapchainSupport =
      deviceVkStn->GetSwapchainSupportDetails(surface);
  const auto &indices = deviceVkStn->GetQueueFamilyIndices(surface);

  VkSurfaceFormatKHR surfaceFormat =
      ChooseSwapSurfaceFormat(swapchainSupport.formats);
  VkPresentModeKHR presentMode =
      ChooseSwapPresentMode(swapchainSupport.presentModes);
  VkExtent2D extent = ChooseSwapExtent(swapchainSupport.capabilities, window);

  uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

  if (swapchainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapchainSupport.capabilities.maxImageCount) {
    imageCount = swapchainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;     // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapchain) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, nullptr);
  m_swapchainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount,
                          m_swapchainImages.data());

  m_swapchainImageFormat = surfaceFormat.format;
  m_swapchainExtent = extent;
}

void SwapchainVk::CreateImageViews(const VkDevice &device) {
  m_swapchainImageViews.resize(m_swapchainImages.size());

  for (size_t i = 0; i < m_swapchainImages.size(); ++i) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = m_swapchainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = m_swapchainImageFormat;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &createInfo, nullptr,
                          &m_swapchainImageViews[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }
  }
};

void SwapchainVk::CleanUp(const VkDevice &device) {
  if (m_swapchainFramebuffers.size() == 0) {
    PC_ERROR("Tried to clear m_swapchainFramebuffers but size is 0!",
             "BasicWorkFlow")
  };

  // Cleanup framebuffers
  for (auto &framebuffer : m_swapchainFramebuffers) {
    PC_VK_NULL_CHECK(device)
    PC_VK_NULL_CHECK(framebuffer)

    vkDestroyFramebuffer(device, framebuffer, nullptr);
    framebuffer = VK_NULL_HANDLE;
  };

  for (auto imageView : m_swapchainImageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }

  vkDestroySwapchainKHR(device, m_swapchain, nullptr);
};

VkSurfaceFormatKHR SwapchainVk::ChooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {

  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR SwapchainVk::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
SwapchainVk::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                              GLFWwindow *window) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {

    if (m_appWin == nullptr) {
      PC_ERROR("m_appWin is nullptr", "SwapchainVk")
    };

    VkExtent2D actualExtent{m_appWin->GetFramebufferSize().first,
                            m_appWin->GetFramebufferSize().second};

    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
    actualExtent.height =
        std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

void SwapchainVk::CreateSwapchainFramebuffers(
    const VkDevice &device, const VkRenderPass &finalRenderPass) {
  auto &swapchainImgViews = GetSwapchainImageViews();
  auto &swapchainExtent = GetSwapchainExtent();

  m_swapchainFramebuffers.resize(swapchainImgViews.size());

  for (size_t i = 0; i < swapchainImgViews.size(); ++i) {
    VkImageView attachments[] = {swapchainImgViews[i]};

    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = finalRenderPass;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = attachments;
    createInfo.width = swapchainExtent.width;
    createInfo.height = swapchainExtent.height;
    createInfo.layers = 1;
    createInfo.pNext = VK_NULL_HANDLE;

    if (vkCreateFramebuffer(device, &createInfo, nullptr,
                            &m_swapchainFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  };
};

void SwapchainVk::RecreateSwapchain(const VkRenderPass &renderPass) {
  auto &device = DeviceVk::Get()->GetDevice();

  uint32_t width = m_appWin->GetFramebufferSize().first,
           height = m_appWin->GetFramebufferSize().second;

  //
  // If the width or height is zero, wait in the loop until it's not
  while (width == 0 || height == 0) {
    width = m_appWin->GetFramebufferSize().first;
    height = m_appWin->GetFramebufferSize().second;

    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device);
  CleanUp(device);

  CreateSwapchainImagesAndVkSwapchain();
  CreateImageViews(device);
  CreateSwapchainFramebuffers(device, renderPass);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
