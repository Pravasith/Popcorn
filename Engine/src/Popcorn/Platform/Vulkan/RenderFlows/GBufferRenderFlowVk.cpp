#include "RenderFlows/GBufferRenderFlowVk.h"
#include "ContextVk.h"
#include "FramebuffersVk.h"
#include "ImageVk.h"
#include "RenderPassVk.h"
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void GBufferRenderFlowVk::CreateAttachments() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  const auto &width = swapchainExtent.width;
  const auto &height = swapchainExtent.height;

  const auto albedoFormat = VK_FORMAT_R8G8B8A8_UNORM;
  const auto depthFormat = VK_FORMAT_D32_SFLOAT;
  const auto normalFormat = VK_FORMAT_A2B10G10R10_UNORM_PACK32;

  //
  //
  // --- Create G-Buffer Images ------------------------------------------------
  VkImageCreateInfo albedoMapInfo;
  ImageVk::GetDefaultImageCreateInfo(albedoMapInfo, width, height);
  albedoMapInfo.format = albedoFormat;
  albedoMapInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VkImageCreateInfo depthMapInfo;
  ImageVk::GetDefaultImageCreateInfo(depthMapInfo, width, height);
  depthMapInfo.format = depthFormat;
  depthMapInfo.usage =
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VkImageCreateInfo normalMapInfo;
  ImageVk::GetDefaultImageCreateInfo(normalMapInfo, width, height);
  normalMapInfo.format = normalFormat;
  normalMapInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VmaAllocationCreateInfo albedoAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo depthAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo normalAlloc{.usage = VMA_MEMORY_USAGE_AUTO};

  ImageVk &albedoImage = m_gBuffer.albedoMap;
  ImageVk &depthImage = m_gBuffer.depthMap;
  ImageVk &normalImage = m_gBuffer.normalMap;

  albedoImage.CreateVmaImage(albedoMapInfo, albedoAlloc);
  depthImage.CreateVmaImage(depthMapInfo, depthAlloc);
  normalImage.CreateVmaImage(normalMapInfo, normalAlloc);

  //
  //
  // --- Create G-Buffer Image Views -------------------------------------------
  VkImageViewCreateInfo albedoViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(albedoViewInfo,
                                         albedoImage.GetVkImage());
  albedoViewInfo.format = albedoFormat;

  VkImageViewCreateInfo depthViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(depthViewInfo,
                                         depthImage.GetVkImage());
  depthViewInfo.format = depthFormat;

  VkImageViewCreateInfo normalViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(normalViewInfo,
                                         normalImage.GetVkImage());
  normalViewInfo.format = normalFormat;

  albedoImage.CreateImageView(albedoViewInfo);
  depthImage.CreateImageView(depthViewInfo);
  normalImage.CreateImageView(normalViewInfo);

  //
  //
  // --- Create G-Buffer Attachments -------------------------------------------
  VkAttachmentDescription albedoAttachment{};
  RenderPassVk::GetDefaultAttachmentDescription(albedoAttachment);
  albedoAttachment.format = albedoFormat;
  albedoAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  RenderPassVk::GetDefaultAttachmentDescription(depthAttachment);
  depthAttachment.format = depthFormat;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

  VkAttachmentDescription normalAttachment{};
  RenderPassVk::GetDefaultAttachmentDescription(normalAttachment);
  normalAttachment.format = normalFormat;
  normalAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  albedoImage.SetAttachmentDescription(albedoAttachment);
  depthImage.SetAttachmentDescription(depthAttachment);
  normalImage.SetAttachmentDescription(normalAttachment);
};

void GBufferRenderFlowVk::CreateFramebuffer() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  std::vector<VkImageView> attachments = {
      m_gBuffer.albedoMap.GetVkImageView(),
      m_gBuffer.depthMap.GetVkImageView(),
      m_gBuffer.normalMap.GetVkImageView(),
  };

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

void GBufferRenderFlowVk::CleanUp() {
  const VkDevice &device = ContextVk::Device()->GetDevice();

  //
  // --- Destroy Framebuffer ---------------------------------------------------
  if (m_framebuffer != VK_NULL_HANDLE) {
    FramebuffersVk::DestroyVkFramebuffer(device, m_framebuffer);
    m_framebuffer = VK_NULL_HANDLE;
  }

  //
  // --- Destroy Renderpass ----------------------------------------------------
  if (m_renderPass.GetVkRenderPass() != VK_NULL_HANDLE) {
    m_renderPass.Destroy(device);
  }

  //
  // --- Destroy G-Buffer Attachments ------------------------------------------
  m_gBuffer.albedoMap.Destroy();
  m_gBuffer.depthMap.Destroy();
  m_gBuffer.normalMap.Destroy();
}

void GBufferRenderFlowVk::CreateRenderPass() {
  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription attachments[]{
      m_gBuffer.albedoMap.GetAttachmentDescription(),
      m_gBuffer.depthMap.GetAttachmentDescription(),
      m_gBuffer.normalMap.GetAttachmentDescription()};

  //
  // --- Attachment references -------------------------------------------------
  VkAttachmentReference albedoRef{};
  RenderPassVk::GetAttachmentRef(albedoRef, 0);
  albedoRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthRef{};
  RenderPassVk::GetAttachmentRef(depthRef, 1);
  depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

  VkAttachmentReference normalRef{};
  RenderPassVk::GetAttachmentRef(normalRef, 2);
  normalRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachments[]{albedoRef, normalRef};

  //
  // --- Create Subpasses ------------------------------------------------------
  VkSubpassDescription subpass{};
  RenderPassVk::GetDefaultSubpassDescription(subpass);
  subpass.pColorAttachments = colorAttachments;
  subpass.pDepthStencilAttachment = &depthRef;
  subpass.colorAttachmentCount = 2;
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  VkSubpassDescription subpasses[]{subpass};

  //
  // --- Dependencies ----------------------------------------------------------
  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = 0;

  //
  // --- Renderpass ------------------------------------------------------------
  VkRenderPassCreateInfo renderPassInfo{};
  RenderPassVk::GetDefaultRenderPassCreateInfo(renderPassInfo);
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;
  renderPassInfo.pAttachments = attachments;
  renderPassInfo.pSubpasses = subpasses;
  renderPassInfo.subpassCount = 1;

  m_renderPass.Create(renderPassInfo, ContextVk::Device()->GetDevice());
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
