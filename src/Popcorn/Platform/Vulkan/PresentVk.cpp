#include "PresentVk.h"
#include "CmdPoolVk.h"
#include "Global_Macros.h"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
void PresentVk::CreateSyncObjs(const VkDevice &dev) {
  VkSemaphoreCreateInfo smphInfo{};
  smphInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  if ((vkCreateSemaphore(dev, &smphInfo, nullptr, &m_imgAvailableSmph) !=
       VK_SUCCESS) ||
      (vkCreateSemaphore(dev, &smphInfo, nullptr, &m_renderFinishedSmph) !=
       VK_SUCCESS) ||
      (vkCreateFence(dev, &fenceInfo, nullptr, &m_inFlightFence) !=
       VK_SUCCESS)) {
    throw std::runtime_error("ERROR: FAILED TO CREATE SEMAPHORES & FENCE");
  };
};

void PresentVk::DrawFrame(
    const VkDevice &dev, const CmdPoolVk &CmdPoolVk,
    const VkSwapchainKHR &swpChn, VkCommandBuffer &cmdBfr,
    const VkRenderPass &rndrPass,
    const std::vector<VkFramebuffer> &swpChnFrameBfrs,
    const VkExtent2D &swpChnExt, const VkPipeline &gfxPipeline,
    const VkQueue &gfxQueue, const VkQueue &presentQueue,
    const CmdPoolVk::RecordCmdBfrPtr recordCmdBfrPtr) const {

  // ACQUIRE IMAGE FROM THE SWAP CHAIN
  // USING SEPHAMORES AND FENCES
  vkWaitForFences(dev, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
  vkResetFences(dev, 1, &m_inFlightFence);

  uint32_t imgIdx;
  vkAcquireNextImageKHR(dev, swpChn, UINT64_MAX, m_imgAvailableSmph,
                        VK_NULL_HANDLE, &imgIdx);

  // RECORD COMMAND BUFFER
  vkResetCommandBuffer(cmdBfr, 0);
  // RECOND COMMAND BUFFER POINTER FUNC CALL
  (CmdPoolVk.*recordCmdBfrPtr)(cmdBfr, imgIdx, rndrPass, swpChnFrameBfrs,
                               swpChnExt, gfxPipeline);

  // SUBMIT THE COMMAND BUFFER
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore waitSmphs[] = {m_imgAvailableSmph};

  submitInfo.waitSemaphoreCount = 1;

  // WAIT FOR THE IMAGE AVAILABLE SMPH BEFORE YOU SUBMIT THE CMDBFR
  submitInfo.pWaitSemaphores = waitSmphs;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmdBfr;

  // SIGNAL THE RENDER-FINISHED SMPH WHEN THE RENDER IS FINISHED (CMDBFR EXEC IS
  // DONE), SO THE IMAGE-AVAILABLE SMPH(ABOVE LINES) STOPS WAITING AND THE NEXT
  // CMDBFR STARTS EXECUTING
  VkSemaphore signalSmphs[] = {m_renderFinishedSmph};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSmphs;

  if (vkQueueSubmit(gfxQueue, 1, &submitInfo, m_inFlightFence) != VK_SUCCESS) {
    throw std::runtime_error("FAILED TO SUBMIT DRAW COMMAND BUFFER!");
  };

  // PRESENTATION BITCHHHH
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
};

void PresentVk::CleanUp(const VkDevice &dev) {
  vkDestroySemaphore(dev, m_imgAvailableSmph, nullptr);
  vkDestroySemaphore(dev, m_renderFinishedSmph, nullptr);
  vkDestroyFence(dev, m_inFlightFence, nullptr);
};

ENGINE_NAMESPACE_END
