#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
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

  void CreateRenderSyncObjects();
  void AcquireSwapchainImage();

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

  VkSemaphore m_imageAvailableSemaphore = VK_NULL_HANDLE;
  VkSemaphore m_frameRenderedSemaphore = VK_NULL_HANDLE;
  VkFence m_inFlightFence = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
