#include "RenderFlows/CompositeRenderFlowVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "RenderPassVk.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// --- CREATE ATTACHMENTS ------------------------------------------------------
// --- CREATE ATTACHMENTS ------------------------------------------------------
// --- CREATE ATTACHMENTS ------------------------------------------------------
//
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
  presentImageAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  presentImageRef.SetAttachmentDescription(presentImageAttachment);
}

//
//
//
//
//
// --- CREATE RENDER PASS ------------------------------------------------------
// --- CREATE RENDER PASS ------------------------------------------------------
// --- CREATE RENDER PASS ------------------------------------------------------
//
void CompositeRenderFlowVk::CreateRenderPass() {
  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription attachments[]{
      m_attachments.presentImage.GetAttachmentDescription()};

  //
  // --- Attachment references -------------------------------------------------
  VkAttachmentReference finalAttachmentRef{};
  RenderPassVk::GetAttachmentRef(finalAttachmentRef, 0);
  finalAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference attachmentRefs[]{finalAttachmentRef};

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

  //
  // --- Renderpass ------------------------------------------------------------
  VkRenderPassCreateInfo renderPassInfo{};
  RenderPassVk::GetDefaultRenderPassCreateInfo(renderPassInfo);
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = attachments;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = subpasses;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  m_renderPass.Create(renderPassInfo, ContextVk::Device()->GetDevice());
};

//
//
//
//
//
// --- CREATE FRAMEBUFFER ------------------------------------------------------
// --- CREATE FRAMEBUFFER ------------------------------------------------------
// --- CREATE FRAMEBUFFER ------------------------------------------------------
//
void CompositeRenderFlowVk::CreateFramebuffer() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  std::vector<VkImageView> attachments{
      m_attachments.presentImage.GetVkImageView()};

  VkFramebufferCreateInfo createInfo{};
  FramebuffersVk::GetDefaultFramebufferState(createInfo);
  createInfo.renderPass = m_renderPass.GetVkRenderPass();
  createInfo.pAttachments = attachments.data();
  createInfo.attachmentCount = attachments.size();
  createInfo.width = swapchainExtent.width;
  createInfo.height = swapchainExtent.height;
  createInfo.layers = 1;

  FramebuffersVk::CreateVkFramebuffer(ContextVk::Device()->GetDevice(),
                                      createInfo, m_framebuffer);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
