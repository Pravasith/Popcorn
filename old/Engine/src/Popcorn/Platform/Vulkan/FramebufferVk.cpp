#include "FramebufferVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

FramebufferVk *FramebufferVk::s_instance = nullptr;

//
// -------------------------------------------------------------------------
// --- PUBLIC METHODS ------------------------------------------------------
void FramebufferVk::CreateVkFramebuffer(
    const VkDevice &device, const VkFramebufferCreateInfo &createInfo,
    VkFramebuffer &framebuffer) {
  if (vkCreateFramebuffer(device, &createInfo, nullptr, &framebuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create framebuffer!");
  }
};

void FramebufferVk::DestroyVkFramebuffer(const VkDevice &device,
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
