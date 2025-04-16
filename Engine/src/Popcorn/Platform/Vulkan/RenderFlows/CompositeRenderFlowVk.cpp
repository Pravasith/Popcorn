#include "RenderFlows/CompositeRenderFlowVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "RenderPassVk.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void CompositeRenderFlowVk::CreateAttachments() {
  const auto &swapchain = ContextVk::Swapchain();

  const auto &format = swapchain->GetSwapchainImageFormat();

  //
  // --- Create Images ---------------------------------------------------------
  VkImageCreateInfo presentImageInfo;
  ImageVk::GetDefaultImageCreateInfo(presentImageInfo,
                                     swapchain->GetSwapchainExtent().width,
                                     swapchain->GetSwapchainExtent().height);
  presentImageInfo.format = format;
  presentImageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  VmaAllocationCreateInfo presentImageAlloc{.usage = VMA_MEMORY_USAGE_AUTO};

  ImageVk &presentImageRef = m_attachments.presentImage;
  presentImageRef.CreateVmaImage(presentImageInfo, presentImageAlloc);

  //
  // --- Image views -----------------------------------------------------------
  VkImageViewCreateInfo presentImageViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(presentImageViewInfo,
                                         presentImageRef.GetVkImage());
  presentImageViewInfo.format = format;
  presentImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  presentImageRef.CreateImageView(presentImageViewInfo);

  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription presentImageAttachment{};
  RenderPassVk::GetDefaultAttachmentDescription(presentImageAttachment);
  presentImageAttachment.format = format;
  presentImageAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  presentImageAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  presentImageRef.SetAttachmentDescription(presentImageAttachment);
}

void CompositeRenderFlowVk::CreateRenderPass() {
  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription attachments[]{
      m_attachments.presentImage.GetAttachmentDescription()};

  //
  // --- Attachment references -------------------------------------------------
  VkAttachmentReference lightAttachmentRef{};
  RenderPassVk::GetAttachmentRef(lightAttachmentRef, 0);
  lightAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference attachmentRefs[]{lightAttachmentRef};

  //
  // --- Create Subpasses ------------------------------------------------------
  VkSubpassDescription subpassInfo{};
  RenderPassVk::GetDefaultSubpassDescription(subpassInfo);
  subpassInfo.pColorAttachments = attachmentRefs;
  subpassInfo.colorAttachmentCount = 1;
  subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  VkSubpassDescription subpasses[]{subpassInfo};

  //
  // --- Dependencies ----------------------------------------------------------
  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  {
    // // Composite pass
    // VkSubpassDependency dependency{};
    // dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    // dependency.dstSubpass = 0;
    // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    // dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    // dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    // dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
