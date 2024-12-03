#pragma once

#include "CmdPoolVk.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class PresentVk {
  friend class RendererVk;

  PresentVk() { PC_PRINT("CREATED", TagType::Constr, "PRESENT-VK") };
  ~PresentVk() { PC_PRINT("DESTROYED", TagType::Destr, "PRESENT-VK") };

  void DrawFrame(const VkDevice &dev, const CmdPoolVk &CmdPoolVk,
                 const VkSwapchainKHR &swpChn,
                 std::vector<VkCommandBuffer> &cmdBfrs,
                 const VkRenderPass &rndrPass,
                 const std::vector<VkFramebuffer> &swpChnFrameBfrs,
                 const VkExtent2D &swpChnExt, const VkPipeline &gfxPipeline,
                 const VkQueue &gfxQueue, const VkQueue &presentQueue,
                 const CmdPoolVk::RecordCmdBfrPtr recordCmdBfrPtr) const;
  void CreateSyncObjs(const VkDevice &);
  void CleanUp(const VkDevice &);

private:
  // WE HAVE 2 FRAMES IN FLIGHT SO EVERY FRAME HAS IT'S OWN SET OF SMPHS &
  // FENCES, HENCE THE VECTORS
  std::vector<VkSemaphore> m_imgAvailableSmphs;
  std::vector<VkSemaphore> m_renderFinishedSmphs;
  std::vector<VkFence> m_inFlightFences;

  static uint32_t s_currFrame;
};

ENGINE_NAMESPACE_END
