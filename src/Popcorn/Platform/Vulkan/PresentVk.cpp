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

  VkSemaphore waitSmphs[] = {m_imgAvailableSmph};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSmphs;
  submitInfo.pWaitDstStageMask = waitStages;
};

void PresentVk::CleanUp(const VkDevice &dev) {
  vkDestroySemaphore(dev, m_imgAvailableSmph, nullptr);
  vkDestroySemaphore(dev, m_renderFinishedSmph, nullptr);
  vkDestroyFence(dev, m_inFlightFence, nullptr);
};

ENGINE_NAMESPACE_END
