#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// Singleton
class FramebuffersVk {
public:
  [[nodiscard]] static inline FramebuffersVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new FramebuffersVk();
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

  void GetDefaultFramebufferState(VkFramebufferCreateInfo &createInfo) const {
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = VK_NULL_HANDLE;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = VK_NULL_HANDLE;
    createInfo.width = 100;
    createInfo.height = 100;
    createInfo.layers = 1;
    createInfo.pNext = VK_NULL_HANDLE;
  };

  void CreateVkFramebuffer(const VkDevice &device,
                           const VkFramebufferCreateInfo &createInfo,
                           VkFramebuffer &framebuffer);
  void DestroyVkFramebuffer(const VkDevice &device, VkFramebuffer &framebuffer);

private:
  FramebuffersVk() { PC_PRINT("CREATED", TagType::Constr, "FramebuffersVk") };
  ~FramebuffersVk() { PC_PRINT("DESTROYED", TagType::Destr, "FramebuffersVk") };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  FramebuffersVk(const FramebuffersVk &) = delete;
  FramebuffersVk &operator=(const FramebuffersVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  FramebuffersVk(FramebuffersVk &&) = delete;
  FramebuffersVk &operator=(FramebuffersVk &&) = delete;

private:
  static FramebuffersVk *s_instance;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
