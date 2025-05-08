#include "RenderFlows/CompositeRenderFlowVk.h"
#include "AttachmentVk.h"
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

  ImageVk &presentImageRef = m_imagesVk.presentImage;
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
  AttachmentVk::GetDefaultAttachmentDescription(presentImageAttachment);
  presentImageAttachment.format = format;
  presentImageAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  presentImageAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  m_attachmentsVk.presentAttachment.SetImageVk(&presentImageRef);
  m_attachmentsVk.presentAttachment.SetAttachmentDescription(
      presentImageAttachment);
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
      m_attachmentsVk.presentAttachment.GetAttachmentDescription()};

  //
  // --- Attachment references -------------------------------------------------
  VkAttachmentReference finalAttachmentRef{};
  AttachmentVk::GetAttachmentRef(finalAttachmentRef, 0);
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
      m_imagesVk.presentImage.GetVkImageView()};

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

//
//
//
//
//
// --- CREATE DESCRIPTORS ------------------------------------------------------
// --- CREATE DESCRIPTORS ------------------------------------------------------
// --- CREATE DESCRIPTORS ------------------------------------------------------
//
void CompositeRenderFlowVk::CreateAndAllocDescriptors() {
  auto *pools = ContextVk::DescriptorPools();

  VkDescriptorSetLayout &compositeLayout =
      ContextVk::DescriptorLayouts()->GetLayout<DescriptorSets::CompositeSet>();

  std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> compositeLayouts{};
  std::fill(compositeLayouts.begin(), compositeLayouts.end(), compositeLayout);

  DPoolVk &compositePool = pools->GetPool<DescriptorPools::CompositePool>();

  // Creates multiple sets (from Count template param)
  std::vector<VkDescriptorSet> compositeSets =
      compositePool.AllocateDescriptorSets<DescriptorSets::CompositeSet,
                                           MAX_FRAMES_IN_FLIGHT>(
          ContextVk::Device()->GetDevice(), compositeLayouts);
};

//
//
//
//
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
//
void CompositeRenderFlowVk::DestroyFramebuffer() {
  if (m_framebuffer != VK_NULL_HANDLE) {
    FramebuffersVk::DestroyVkFramebuffer(ContextVk::Device()->GetDevice(),
                                         m_framebuffer);
    m_framebuffer = VK_NULL_HANDLE;
  }
};

void CompositeRenderFlowVk::DestroyRenderPass() {
  if (m_renderPass.GetVkRenderPass() != VK_NULL_HANDLE) {
    m_renderPass.Destroy(ContextVk::Device()->GetDevice());
  }
};

void CompositeRenderFlowVk::DestroyAttachments() {
  m_imagesVk.presentImage.Destroy();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
