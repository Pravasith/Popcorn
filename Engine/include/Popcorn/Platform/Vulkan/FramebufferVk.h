#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// Singleton
class FramebufferVk {
public:
  [[nodiscard]] static inline FramebufferVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new FramebufferVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of FramebuffersVk")
    };
  };

  static void GetDefaultFramebufferState(VkFramebufferCreateInfo &createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = VK_NULL_HANDLE;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = VK_NULL_HANDLE;
    createInfo.width = 100;
    createInfo.height = 100;
    createInfo.layers = 1;
    createInfo.pNext = VK_NULL_HANDLE;
  };

  static void CreateVkFramebuffer(const VkDevice &device,
                                  const VkFramebufferCreateInfo &createInfo,
                                  VkFramebuffer &framebuffer);
  static void DestroyVkFramebuffer(const VkDevice &device,
                                   VkFramebuffer &framebuffer);

private:
  FramebufferVk() { PC_PRINT("CREATED", TagType::Constr, "FramebuffersVk") };
  ~FramebufferVk() { PC_PRINT("DESTROYED", TagType::Destr, "FramebuffersVk") };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  FramebufferVk(const FramebufferVk &) = delete;
  FramebufferVk &operator=(const FramebufferVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  FramebufferVk(FramebufferVk &&) = delete;
  FramebufferVk &operator=(FramebufferVk &&) = delete;

private:
  static FramebufferVk *s_instance;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
