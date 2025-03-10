#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <functional>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// Singleton
class FrameVk {
public:
  [[nodiscard]] static inline FrameVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new FrameVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of FrameVk")
    };
  };

  void
  Draw(std::vector<VkCommandBuffer> &commandBuffers,
       const VkRenderPass &finalPaintRenderPass,
       const std::function<void(const uint32_t currentFrame)> &updateSceneData,
       const std::function<void(const uint32_t frameIndex,
                                VkCommandBuffer &currentFrameCommandBuffer)>
           &recordDrawCommands);

  void CreateRenderSyncObjects();
  inline void SetFrameBufferResized(bool isFrameBufferResized) {
    m_framebufferResized = isFrameBufferResized;
  };
  void CleanUp();

private:
  [[nodiscard]] bool
  AcquireNextSwapchainImageIndex(uint32_t &imageIndex,
                                 VkSemaphore *signalSemaphores,
                                 const VkRenderPass &paintRenderPass);
  void SubmitDrawCommands(const VkCommandBuffer &commandBuffer,
                          VkSemaphore *waitSemaphores,
                          VkSemaphore *signalSemaphores,
                          VkFence &inFlightFence);
  void PresentImageToSwapchain(VkSemaphore *signalSemaphores,
                               const uint32_t &imageIndex,
                               const VkRenderPass &paintRenderPass);

private:
  FrameVk() { PC_PRINT("CREATED", TagType::Constr, "FrameVk") };
  ~FrameVk() { PC_PRINT("DESTROYED", TagType::Destr, "FrameVk") };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  FrameVk(const FrameVk &) = delete;
  FrameVk &operator=(const FrameVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  FrameVk(FrameVk &&) = delete;
  FrameVk &operator=(FrameVk &&) = delete;

private:
  static FrameVk *s_instance;

  uint32_t m_currentFrame = 0;
  bool m_framebufferResized = false;

  std::vector<VkSemaphore> m_imageAvailableSemaphores;
  std::vector<VkSemaphore> m_frameRenderedSemaphores;
  std::vector<VkFence> m_inFlightFences;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
