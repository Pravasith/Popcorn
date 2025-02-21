#include "FramebuffersVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

FramebuffersVk *FramebuffersVk::s_instance = nullptr;

//
// -------------------------------------------------------------------------
// --- PUBLIC METHODS ------------------------------------------------------
void FramebuffersVk::CreateVkFramebuffer(
    const VkDevice &device, const VkFramebufferCreateInfo &createInfo,
    VkFramebuffer &framebuffer) {
  if (vkCreateFramebuffer(device, &createInfo, nullptr, &framebuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create framebuffer!");
  }
};

void FramebuffersVk::DestroyVkFramebuffer(const VkDevice &device,
                                          VkFramebuffer &framebuffer) {
  PC_VK_NULL_CHECK(device)
  PC_VK_NULL_CHECK(framebuffer)

  vkDestroyFramebuffer(device, framebuffer, nullptr);
};

//
// -------------------------------------------------------------------------
// --- PRIVATE METHODS -----------------------------------------------------

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
