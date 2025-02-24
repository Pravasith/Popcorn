#include "FrameVk.h"
#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "SwapchainVk.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

FrameVk *FrameVk::s_instance = nullptr;
uint32_t FrameVk::s_swapchainImageIndex = 0;

void FrameVk::CreateRenderSyncObjects() {
  auto *deviceVkStn = DeviceVk::Get();
  auto &device = deviceVkStn->GetDevice();

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &m_imageAvailableSemaphore) != VK_SUCCESS ||
      vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                        &m_frameRenderedSemaphore) != VK_SUCCESS ||
      vkCreateFence(device, &fenceInfo, nullptr, &m_inFlightFence) !=
          VK_SUCCESS) {
    throw std::runtime_error("failed to create semaphores!");
  }
};

void FrameVk::Draw(
    VkCommandBuffer &commandBuffer,
    const std::function<void(const uint32_t frameIndex)> &recordDrawCommands) {
  auto &device = DeviceVk::Get()->GetDevice();

  //
  // Note: Thread-blocking
  // Image is still in-flight(still in the rendering process)
  vkWaitForFences(device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
  // Once done, we reset the fence to in-flight mode
  vkResetFences(device, 1, &m_inFlightFence);

  VkSemaphore imageAvailable[] = {m_imageAvailableSemaphore};
  VkSemaphore frameRendered[] = {m_frameRenderedSemaphore};

  //
  // Now we acquire a new image from the swapchain and signal m_imageAvailable
  // semaphore once done
  AcquireNextSwapchainImage(
      // index is [0, MAX_FRAMES_IN_FLIGHT]
      s_swapchainImageIndex,
      // Semaphore: Signal when image is acquired & ready for render
      imageAvailable);

  //
  // Reset command buffer & start recording commands to it (lambda called from
  // the RendererVk class)
  vkResetCommandBuffer(commandBuffer, 0);
  recordDrawCommands(s_swapchainImageIndex);

  //
  // Submit the graphics queue with the command buffer (with recorded commands
  // in it)
  SubmitDrawCommands(
      commandBuffer,
      // Semaphore: Wait for the image to be available to paint/render
      imageAvailable,
      // Semaphore: Signal when frame is rendered & ready to present to
      // the screen
      frameRendered);

  //
  // Present the rendered image to the screen - by returning the image back to
  // the swapchain
  PresentImageToSwapchain(
      // Semaphore: Wait for the frame to be fully rendered before presenting
      frameRendered,
      // Image index
      s_swapchainImageIndex);
};

void FrameVk::PresentImageToSwapchain(VkSemaphore *signalSemaphores,
                                      const uint32_t &imageIndex) {
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {SwapchainVk::Get()->GetVkSwapchain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr; // Optional

  vkQueuePresentKHR(DeviceVk::Get()->GetPresentQueue(), &presentInfo);
};

void FrameVk::AcquireNextSwapchainImage(uint32_t &imageIndex,
                                        VkSemaphore *signalSemaphores) {
  auto &device = DeviceVk::Get()->GetDevice();
  auto &swapchain = SwapchainVk::Get()->GetVkSwapchain();

  vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, *signalSemaphores,
                        VK_NULL_HANDLE, &imageIndex);
};

void FrameVk::SubmitDrawCommands(const VkCommandBuffer &commandBuffer,
                                 VkSemaphore *waitSemaphores,
                                 VkSemaphore *signalSemaphores) {
  auto &graphicsQueue = DeviceVk::Get()->GetGraphicsQueue();

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  //
  // Wait at the color attachment output stage - bc the subpass dependency will
  // make this happen
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;
  //
  // Signal the m_frameRendererdSemaphore once the frame is rendered to the
  // provided image
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, m_inFlightFence) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }
};

void FrameVk::CleanUp() {
  auto &device = DeviceVk::Get()->GetDevice();

  vkDestroySemaphore(device, m_imageAvailableSemaphore, nullptr);
  vkDestroySemaphore(device, m_frameRenderedSemaphore, nullptr);
  vkDestroyFence(device, m_inFlightFence, nullptr);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
