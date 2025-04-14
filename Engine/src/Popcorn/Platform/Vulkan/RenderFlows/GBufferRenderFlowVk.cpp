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
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VkImageCreateInfo normalMapInfo;
  ImageVk::GetDefaultImageCreateInfo(normalMapInfo, width, height);
  normalMapInfo.format = normalFormat;
  normalMapInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VmaAllocationCreateInfo albedoAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo depthAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo normalAlloc{.usage = VMA_MEMORY_USAGE_AUTO};

  ImageVk albedoImage{};
  ImageVk depthImage{};
  ImageVk normalImage{};

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
};

void GBufferRenderFlowVk::CreateFramebuffer() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  std::vector<VkImageView> attachments = {
      m_gBufferData.albedoMap.GetVkImageView(),
      m_gBufferData.depthMap.GetVkImageView(),
      m_gBufferData.normalMap.GetVkImageView(),
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
    vkDestroyFramebuffer(device, m_framebuffer, nullptr);
    m_framebuffer = VK_NULL_HANDLE;
  }

  //
  // --- Destroy G-Buffer Attachments ------------------------------------------
  m_gBufferData.albedoMap.Destroy();
  m_gBufferData.depthMap.Destroy();
  m_gBufferData.normalMap.Destroy();
}

void GBufferRenderFlowVk::CreateRenderPass() {

};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
