#pragma once

#include "CmdPoolVk.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class PresentVk {
  friend class RendererVk;

  PresentVk() { PC_PRINT("CREATED", TagType::Constr, "PRESENT-VK") };
  ~PresentVk() { PC_PRINT("DESTROYED", TagType::Destr, "PRESENT-VK") };

  void DrawFrame(const VkDevice &dev, CmdPoolVk &CmdPoolVk,
                 const VkSwapchainKHR &swpChn, VkCommandBuffer &cmdBfr,
                 const VkRenderPass &rndrPass,
                 const std::vector<VkFramebuffer> &swpChnFrameBfrs,
                 const VkExtent2D &swpChnExt, const VkPipeline &gfxPipeline,
                 const CmdPoolVk::RecordCmdBfrPtr recordCmdBfrPtr);
  void CreateSyncObjs(const VkDevice &);
  void CleanUp(const VkDevice &);

private:
  VkSemaphore m_imgAvailableSmph;
  VkSemaphore m_renderFinishedSmph;
  VkFence m_inFlightFence;
};
ENGINE_NAMESPACE_END
