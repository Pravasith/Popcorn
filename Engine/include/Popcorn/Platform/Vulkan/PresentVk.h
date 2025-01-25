#pragma once

#include "CmdPoolVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "SwapChainVk.h"
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
class PresentVk {
  friend class RendererVk;

  PresentVk(const VkDevice &logiDevice, const VkSwapchainKHR &swpChn,
            const VkQueue &gfxQueue, const VkQueue &presentQueue,
            std::pair<const uint32_t &, const uint32_t &> frmBfrSize)
      : m_logiDevice(logiDevice), m_swpChn(swpChn), m_gfxQueue(gfxQueue),
        m_presentQueue(presentQueue) {
          PC_PRINT("CREATED", TagType::Constr, "PRESENT-VK")
        };
  ~PresentVk() { PC_PRINT("DESTROYED", TagType::Destr, "PRESENT-VK") };

  // inline void SetFrameBfrSize (std::pair(const uint32_t))
  inline void SetFrameBfrResized(bool isResized) {
    s_frameBfrResized = isResized;
  };

  void DrawFrame(std::vector<VkCommandBuffer> &cmdBfrs,
                 CmdPoolVk::RecordCmdBfrFtr recordCmdBfr,
                 SwapChainVk::RecreateSwapChainFtr recreateSwapChain) const;

  void CreateSyncObjs(const VkDevice &);
  void CleanUp(const VkDevice &);

private:
  static uint32_t s_currFrame;

  // WE HAVE 2 FRAMES IN FLIGHT SO EVERY FRAME HAS IT'S OWN SET OF SMPHS &
  // FENCES, HENCE THE VECTORS
  std::vector<VkSemaphore> m_imgAvailableSmphs;
  std::vector<VkSemaphore> m_renderFinishedSmphs;
  std::vector<VkFence> m_inFlightFences;

  static bool s_frameBfrResized;

  // REFERENCES
  const VkDevice &m_logiDevice;
  const VkSwapchainKHR &m_swpChn;
  const VkQueue &m_gfxQueue;
  const VkQueue &m_presentQueue;
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
