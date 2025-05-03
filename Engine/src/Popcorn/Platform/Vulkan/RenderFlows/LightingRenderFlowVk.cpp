#include "RenderFlows/LightingRenderFlowVk.h"
#include "ContextVk.h"
#include "DescriptorLayoutsVk.h"
#include "DescriptorPoolsVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "RenderPassVk.h"
#include <array>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// --- CREATE ATTACHMENTS ------------------------------------------------------
// --- CREATE ATTACHMENTS ------------------------------------------------------
// --- CREATE ATTACHMENTS ------------------------------------------------------
//
void LightingRenderFlowVk::CreateAttachments() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  const auto &format = VK_FORMAT_R16G16B16A16_SFLOAT;

  //
  // --- Create Images ---------------------------------------------------------
  VkImageCreateInfo lightBufferInfo;
  ImageVk::GetDefaultImageCreateInfo(lightBufferInfo, swapchainExtent.width,
                                     swapchainExtent.height);
  lightBufferInfo.format = format;
  lightBufferInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VmaAllocationCreateInfo lightBufferAlloc{.usage = VMA_MEMORY_USAGE_AUTO};

  ImageVk &lightBufferRef = m_attachments.lightBuffer;
  lightBufferRef.CreateVmaImage(lightBufferInfo, lightBufferAlloc);

  //
  // --- Image views -----------------------------------------------------------
  VkImageViewCreateInfo lightBufferViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(lightBufferViewInfo,
                                         lightBufferRef.GetVkImage());
  lightBufferViewInfo.format = format;
  lightBufferViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  lightBufferRef.CreateImageView(lightBufferViewInfo);

  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription lightBufferAttachment{};
  RenderPassVk::GetDefaultAttachmentDescription(lightBufferAttachment);
  lightBufferAttachment.format = format;
  lightBufferAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  lightBufferAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  lightBufferRef.SetAttachmentDescription(lightBufferAttachment);
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
void LightingRenderFlowVk::CreateRenderPass() {
  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription attachments[]{
      m_attachments.lightBuffer.GetAttachmentDescription()};

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
void LightingRenderFlowVk::CreateFramebuffer() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  std::vector<VkImageView> attachments{
      m_attachments.lightBuffer.GetVkImageView()};

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

void LightingRenderFlowVk::CreateAndAllocDescriptors() {
  auto *pools = ContextVk::DescriptorPools();

  VkDescriptorSetLayout &lightingLayout =
      ContextVk::DescriptorLayouts()->GetLayout<DescriptorSets::LightingSet>();

  std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> lightingLayouts{};
  std::fill(lightingLayouts.begin(), lightingLayouts.end(), lightingLayout);

  DPoolVk &lightsPool = pools->GetPool<DescriptorPools::LightingPool>();

  // Creates multiple sets (from Count template param)
  std::vector<VkDescriptorSet> lightingSets =
      lightsPool.AllocateDescriptorSets<DescriptorSets::LightingSet,
                                        MAX_FRAMES_IN_FLIGHT>(
          ContextVk::Device()->GetDevice(), lightingLayouts);
};

//
//
//
//
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
//
void LightingRenderFlowVk::DestroyFramebuffer() {
  if (m_framebuffer != VK_NULL_HANDLE) {
    FramebuffersVk::DestroyVkFramebuffer(ContextVk::Device()->GetDevice(),
                                         m_framebuffer);
    m_framebuffer = VK_NULL_HANDLE;
  }
};

void LightingRenderFlowVk::DestroyRenderPass() {
  if (m_renderPass.GetVkRenderPass() != VK_NULL_HANDLE) {
    m_renderPass.Destroy(ContextVk::Device()->GetDevice());
  }
};

void LightingRenderFlowVk::DestroyAttachments() {
  m_attachments.lightBuffer.Destroy();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
