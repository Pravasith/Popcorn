#include "PresentVk.h"
#include "CmdPoolVk.h"
#include "CommonVk.h"
#include "Global_Macros.h"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
uint32_t PresentVk::s_currFrame = 0;

void PresentVk::CreateSyncObjs(const VkDevice &dev) {
  VkSemaphoreCreateInfo smphInfo{};
  smphInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  m_imgAvailableSmphs.resize(MAX_FRAMES_IN_FLIGHT);
  m_renderFinishedSmphs.resize(MAX_FRAMES_IN_FLIGHT);
  m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    if ((vkCreateSemaphore(dev, &smphInfo, nullptr, &m_imgAvailableSmphs[i]) !=
         VK_SUCCESS) ||
        (vkCreateSemaphore(dev, &smphInfo, nullptr,
                           &m_renderFinishedSmphs[i]) != VK_SUCCESS) ||
        (vkCreateFence(dev, &fenceInfo, nullptr, &m_inFlightFences[i]) !=
         VK_SUCCESS)) {
      throw std::runtime_error("ERROR: FAILED TO CREATE SEMAPHORES & FENCE");
    };
  };
};

void PresentVk::DrawFrame(
    const VkDevice &dev, const CmdPoolVk &CmdPoolVk,
    const VkSwapchainKHR &swpChn, std::vector<VkCommandBuffer> &cmdBfrs,
    const VkRenderPass &rndrPass,
    const std::vector<VkFramebuffer> &swpChnFrameBfrs,
    const VkExtent2D &swpChnExt, const VkPipeline &gfxPipeline,
    const VkQueue &gfxQueue, const VkQueue &presentQueue,
    const CmdPoolVk::RecordCmdBfrPtr recordCmdBfrPtr) const {
  // ACQUIRE IMAGE FROM THE SWAP CHAIN
  // USING SEPHAMORES AND FENCES
  // SYNC CODE - WAIT UNTIL m_inFlightFences[s_currFrame] IS SIGNALLED - (IT'S
  // SIGNALLED WHEN THE CMD BFR HAS BEEN SUBMITTED TO THE SWAPCHAIN)
  vkWaitForFences(dev, 1, &m_inFlightFences[s_currFrame], VK_TRUE, UINT64_MAX);
  vkResetFences(dev, 1, &m_inFlightFences[s_currFrame]);

  // CPU ISSUES ACQ. IMG CALL TO GPU, m_imgAvlSmphs IS SIGNALLED WHEN THE IMG IS
  // READY. THE NEXT GPU INSTRUCTION IS BLOCKED UNTIL THE m_imgAvlSmphs IS
  // SIGNALLED
  uint32_t imgIdx;
  vkAcquireNextImageKHR(dev, swpChn, UINT64_MAX,
                        m_imgAvailableSmphs[s_currFrame], VK_NULL_HANDLE,
                        &imgIdx);

  vkResetCommandBuffer(cmdBfrs[s_currFrame], 0);
  // RECORD COMMAND BUFFER POINTER FUNC CALL
  //
  (CmdPoolVk.*recordCmdBfrPtr)(cmdBfrs[s_currFrame], imgIdx, rndrPass,
                               swpChnFrameBfrs, swpChnExt, gfxPipeline);

  // SUBMIT THE COMMAND BUFFER
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  // WAIT FOR THE IMAGE AVAILABLE SMPH BEFORE YOU SUBMIT THE CMDBFR
  VkSemaphore waitSmphs[] = {m_imgAvailableSmphs[s_currFrame]};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSmphs;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmdBfrs[s_currFrame];

  VkSemaphore signalSmphs[] = {m_renderFinishedSmphs[s_currFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSmphs;

  if (vkQueueSubmit(gfxQueue, 1, &submitInfo, m_inFlightFences[s_currFrame]) !=
      VK_SUCCESS) {
    throw std::runtime_error("FAILED TO SUBMIT DRAW COMMAND BUFFER!");
  };

  // PRESENTATION
  VkPresentInfoKHR prsntInfo{};
  prsntInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  prsntInfo.waitSemaphoreCount = 1;
  prsntInfo.pWaitSemaphores = signalSmphs;

  VkSwapchainKHR swpChns[] = {swpChn};
  prsntInfo.swapchainCount = 1;
  prsntInfo.pSwapchains = swpChns;
  prsntInfo.pImageIndices = &imgIdx;
  prsntInfo.pResults = nullptr; // OPTIONAL

  vkQueuePresentKHR(presentQueue, &prsntInfo);

  s_currFrame = (s_currFrame + 1) % MAX_FRAMES_IN_FLIGHT;
};

void PresentVk::CleanUp(const VkDevice &dev) {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    vkDestroySemaphore(dev, m_imgAvailableSmphs[i], nullptr);
    vkDestroySemaphore(dev, m_renderFinishedSmphs[i], nullptr);
    vkDestroyFence(dev, m_inFlightFences[i], nullptr);
  }
};

ENGINE_NAMESPACE_END
