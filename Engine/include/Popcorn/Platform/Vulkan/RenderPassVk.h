#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class RenderPassVk {
public:
  RenderPassVk() { PC_PRINT("CREATED", TagType::Constr, "RenderPassVk"); };
  ~RenderPassVk() { PC_PRINT("DESTROYED", TagType::Destr, "RenderPassVk"); };

  static void
  GetDefaultAttachmentDescription(VkAttachmentDescription &attachment) {
    // Color attachment
    attachment.format = VK_FORMAT_B8G8R8A8_SRGB; // Ex. Swapchain image format
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  };

  static void GetAttachmentRef(VkAttachmentReference &attachmentRef,
                               const uint32_t attachmentIndex) {
    attachmentRef.attachment = attachmentIndex;
    attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  };

  static void
  GetDefaultSubpassDescription(VkSubpassDescription &subpassDescription) {
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    // subpassDescription.pColorAttachments = &attachmentRefs;
    // subpassDescription.pInputAttachments = &attachmentRefs;
    // subpassDescription.pResolveAttachments = &attachmentRefs;
    // subpassDescription.pResolveAttachments = &attachmentRefs;
    // subpassDescription.pDepthStencilAttachment = &attachmentRefs;
    // subpassDescription.pPreserveAttachments = &preserceAttachmentRefs;
  };

  static void
  SetDefaultRenderPassCreateInfo(VkRenderPassCreateInfo &renderPassInfo) {
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = VK_NULL_HANDLE;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = VK_NULL_HANDLE;
  };

  void Create(const VkRenderPassCreateInfo &renderPassInfo) {
    PC_VK_NULL_CHECK(m_device)
    PC_VK_NULL_CHECK(m_renderPass)

    if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create render pass!");
    };
  };

  void Destroy() {
    PC_VK_NULL_CHECK(m_device)
    PC_VK_NULL_CHECK(m_renderPass)

    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
  };

private:
  VkRenderPass m_renderPass = VK_NULL_HANDLE;
  VkDevice m_device = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
