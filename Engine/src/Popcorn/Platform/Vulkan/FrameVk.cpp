#include "FrameVk.h"
#include "CommonVk.h"
#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "RenderFlowDefs.h"
#include "SwapchainVk.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

FrameVk *FrameVk::s_instance = nullptr;

void FrameVk::CreateRenderSyncObjects() {
  auto *deviceVkStn = DeviceVk::Get();
  auto &device = deviceVkStn->GetDevice();

  m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_frameRenderedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                          &m_frameRenderedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &m_inFlightFences[i]) !=
            VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores!");
    }
  }
};

void FrameVk::Draw(
    const std::function<void()> &swapchainInvalidCb,
    const std::function<void(const uint32_t currentFrame)> &updateSceneDataCb,
    const std::function<void(const uint32_t swapchainFrameIndex,
                             const uint32_t currentFrame)>
        &recordDrawCommandsCb,
    PcRenderFlowCmdBuffersMap &renderFlowCmdBuffersMap) {
  auto &device = DeviceVk::Get()->GetDevice();
  uint32_t swapchainImageIndex;

  //
  // Note: Thread-blocking
  // TODO: Multi-threaded in the future
  // Image is still in-flight(still in the rendering process)
  vkWaitForFences(device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE,
                  UINT64_MAX);

  VkSemaphore imageAvailable[] = {m_imageAvailableSemaphores[m_currentFrame]};
  VkSemaphore frameRendered[] = {m_frameRenderedSemaphores[m_currentFrame]};

  //
  // Now we acquire a new image (it's index) from the swapchain and signal
  // m_imageAvailable semaphore once done
  bool isImageAcquired = AcquireNextSwapchainImageIndex(
      swapchainImageIndex,
      // Semaphore: Signal when image is acquired & ready for render
      imageAvailable,
      // Final paint renderpass for swapchain recreation
      swapchainInvalidCb);

  //
  // In case of resize (or invalid swapchain to be precise), we return and
  // continue from the next frame
  if (!isImageAcquired) {
    // PC_WARN("SWAPCHAIN INVALID")
    vkDeviceWaitIdle(device); // Wait for all operations to finish before
                              // recreating swapchain
    vkResetFences(device, 1, &m_inFlightFences[m_currentFrame]);
    return;
  }

  // Once done, we reset the fence to in-flight mode
  vkResetFences(device, 1, &m_inFlightFences[m_currentFrame]);

  //
  // Update Uniforms & push constants
  updateSceneDataCb(m_currentFrame);

  recordDrawCommandsCb(swapchainImageIndex, m_currentFrame);

  auto &gBufferCmdBfrs = *renderFlowCmdBuffersMap[RenderFlows::GBuffer];
  auto &lightingCmdBfrs = *renderFlowCmdBuffersMap[RenderFlows::Lighting];
  auto &compositeCmdBfrs = *renderFlowCmdBuffersMap[RenderFlows::Composite];

  std::vector<VkCommandBuffer> commandBuffers{gBufferCmdBfrs[m_currentFrame],
                                              lightingCmdBfrs[m_currentFrame],
                                              compositeCmdBfrs[m_currentFrame]};

  //
  // Submit the graphics queue with the command buffer (with recorded commands
  // in it)
  SubmitDrawCommands(
      commandBuffers,
      // Semaphore: Wait for the image to be available to paint/render
      imageAvailable,
      // Semaphore: Signal when frame is rendered & ready to present to
      // the screen
      frameRendered,
      // Fence: Release fence when operation is done
      m_inFlightFences[m_currentFrame]);

  //
  // Present the rendered image to the screen - by returning the image back to
  // the swapchain
  PresentImageToSwapchain(
      // Semaphore: Wait for the frame to be fully rendered before presenting
      frameRendered, swapchainImageIndex, swapchainInvalidCb);

  //
  // Wait until all the commandBuffers are executed before moving to the next
  // steps after the gameloop in the program (usually cleanup)
  // vkDeviceWaitIdle(device); // DON'T NEED THIS. DEFEATS THE PURPOSE OF DOUBLE
  // BUFFERING

  //
  // +1 because we're setting it to next frame, the current frame has already
  // been processed
  m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
};

void FrameVk::PresentImageToSwapchain(
    VkSemaphore *signalSemaphores, const uint32_t &imageIndex,
    const std::function<void()> &swapchainInvalidCb) {
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {SwapchainVk::Get()->GetVkSwapchain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr; // Optional

  VkResult result =
      vkQueuePresentKHR(DeviceVk::Get()->GetPresentQueue(), &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      m_framebufferResized) {
    m_framebufferResized = false;
    swapchainInvalidCb();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }
};

bool FrameVk::AcquireNextSwapchainImageIndex(
    uint32_t &imageIndex, VkSemaphore *signalSemaphores,
    const std::function<void()> &swapchainInvalidCb) {
  auto &device = DeviceVk::Get()->GetDevice();
  auto &swapchain = SwapchainVk::Get()->GetVkSwapchain();

  VkResult result =
      vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, *signalSemaphores,
                            VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    vkDeviceWaitIdle(device);
    swapchainInvalidCb();
    return false;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  return true;
};

void FrameVk::SubmitDrawCommands(std::vector<VkCommandBuffer> &commandBuffers,
                                 VkSemaphore *waitSemaphores,
                                 VkSemaphore *signalSemaphores,
                                 VkFence &inFlightFence) {
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
  submitInfo.commandBufferCount = commandBuffers.size();
  submitInfo.pCommandBuffers = commandBuffers.data();
  //
  // Signal the m_frameRendererdSemaphore once the frame is rendered to the
  // provided image
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }
};

void FrameVk::CleanUp() {
  auto &device = DeviceVk::Get()->GetDevice();
  vkDeviceWaitIdle(device);

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    vkDestroySemaphore(device, m_imageAvailableSemaphores[i], nullptr);
    vkDestroySemaphore(device, m_frameRenderedSemaphores[i], nullptr);
    vkDestroyFence(device, m_inFlightFences[i], nullptr);
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
