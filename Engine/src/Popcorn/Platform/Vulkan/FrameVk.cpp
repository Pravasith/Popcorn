#include "FrameVk.h"
#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "SwapchainVk.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void FrameVk::CreateRenderSyncObjects() {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
};

void FrameVk::AcquireSwapchainImage() {
  auto *deviceVkStn = DeviceVk::Get();
  auto *swapchainVkStn = SwapchainVk::Get();
  static uint32_t g_swapchainImageIndex;

  vkAcquireNextImageKHR(
      deviceVkStn->GetDevice(), swapchainVkStn->GetVkSwapchain(), UINT64_MAX,
      m_imageAvailableSemaphore, VK_NULL_HANDLE, &g_swapchainImageIndex);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
