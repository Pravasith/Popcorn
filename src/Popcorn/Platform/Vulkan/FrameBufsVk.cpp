#include "FrameBufsVk.h"
#include "Global_Macros.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN

void FrameBufsVk::CreateFrameBfrs(
    const VkDevice &dev, const std::vector<VkImageView> &swpChnImgViews,
    const VkRenderPass &rndrPass, const VkExtent2D &swpChnExt) {
  m_swpChnFrameBufs.resize(swpChnImgViews.size());

  for (size_t i = 0; i < swpChnImgViews.size(); ++i) {
    VkImageView attachments[] = {swpChnImgViews[i]};

    VkFramebufferCreateInfo frmbfrInfo{};
    frmbfrInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frmbfrInfo.renderPass = rndrPass;
    frmbfrInfo.attachmentCount = 1;
    frmbfrInfo.pAttachments = attachments;
    frmbfrInfo.width = swpChnExt.width;
    frmbfrInfo.height = swpChnExt.height;
    frmbfrInfo.layers = 1;

    if (vkCreateFramebuffer(dev, &frmbfrInfo, nullptr, &m_swpChnFrameBufs[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("ERROR: FAILED TO CREATE FRAMEBUFFER!");
    };
  };
};

void FrameBufsVk::CleanUp(const VkDevice &dev) {
  for (size_t i = 0; i < m_swpChnFrameBufs.size(); ++i) {
    vkDestroyFramebuffer(dev, m_swpChnFrameBufs[i], nullptr);
  };
};

ENGINE_NAMESPACE_END
