#include "RenderFlows/LightingRenderFlowVk.h"
#include "AttachmentVk.h"
#include "CommonVk.h"
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
  VkImageCreateInfo lightImageInfo;
  ImageVk::GetDefaultImageCreateInfo(lightImageInfo, swapchainExtent.width,
                                     swapchainExtent.height);
  lightImageInfo.format = format;
  lightImageInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VmaAllocationCreateInfo lightImageAlloc{.usage = VMA_MEMORY_USAGE_AUTO};

  ImageVk &lightImageRef = m_imagesVk.lightImage;
  lightImageRef.CreateVmaImage(lightImageInfo, lightImageAlloc);

  //
  // --- Image views -----------------------------------------------------------
  VkImageViewCreateInfo lightImageViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(lightImageViewInfo,
                                         lightImageRef.GetVkImage());
  lightImageViewInfo.format = format;
  lightImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  lightImageRef.CreateImageView(lightImageViewInfo);

  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription lightImageAttachment{};
  AttachmentVk::GetDefaultAttachmentDescription(lightImageAttachment);
  lightImageAttachment.format = format;
  lightImageAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  lightImageAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  m_attachmentsVk.lightAttachment.SetImageVk(&lightImageRef);

  m_attachmentsVk.lightAttachment.SetAttachmentDescription(
      lightImageAttachment);
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
      m_attachmentsVk.lightAttachment.GetAttachmentDescription()};

  //
  // --- Attachment references -------------------------------------------------
  VkAttachmentReference lightAttachmentRef{};
  AttachmentVk::GetAttachmentRef(lightAttachmentRef, 0);
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

  std::vector<VkImageView> attachments{m_imagesVk.lightImage.GetVkImageView()};

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
  auto *device = ContextVk::Device();
  auto *memory = ContextVk::Memory();
  VkPhysicalDeviceProperties properties{};
  device->GetPhysicalDeviceProperties(properties);

  VkDescriptorSetLayout &lightingLayout =
      ContextVk::DescriptorLayouts()->GetLayout<DescriptorSets::LightingSet>();

  std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> lightingLayouts{};
  std::fill(lightingLayouts.begin(), lightingLayouts.end(), lightingLayout);

  DPoolVk &lightsPool =
      pools->GetPool<DescriptorPools::LightingPool>(MAX_FRAMES_IN_FLIGHT);

  // Creates multiple sets (from Count template param)
  m_descriptorSetsVk.lightingSets =
      lightsPool.AllocateDescriptorSets<DescriptorSets::LightingSet,
                                        MAX_FRAMES_IN_FLIGHT>(
          ContextVk::Device()->GetDevice(), lightingLayouts);

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    VkDescriptorBufferInfo lightBufferInfo{};
    lightBufferInfo.buffer = memory->GetUboSet(i);
    lightBufferInfo.offset = memory->GetBufferViews().lightsSsbo.offset;
    lightBufferInfo.range = DescriptorLayoutsVk::GetDescriptorBufferRange<
        DescriptorSets::LightingSet>(properties.limits);

    VkDescriptorImageInfo albedoImageInfo{};
    albedoImageInfo.imageView = m_dependencyImages.albedoImage.GetVkImageView();
    albedoImageInfo.sampler = s_samplersVk.frameSampler.GetVkSampler();
    albedoImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkDescriptorImageInfo depthImageInfo{};
    depthImageInfo.imageView = m_dependencyImages.depthImage.GetVkImageView();
    depthImageInfo.sampler = s_samplersVk.frameSampler.GetVkSampler();
    depthImageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

    VkDescriptorImageInfo normalImageInfo{};
    normalImageInfo.imageView = m_dependencyImages.normalImage.GetVkImageView();
    normalImageInfo.sampler = s_samplersVk.frameSampler.GetVkSampler();
    normalImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet lightWrite{};
    lightWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    lightWrite.dstSet = m_descriptorSetsVk.lightingSets[i];
    lightWrite.dstBinding = 0;
    lightWrite.dstArrayElement = 0;
    lightWrite.descriptorType =
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // is this correct?
    lightWrite.descriptorCount = 1;
    lightWrite.pBufferInfo = &lightBufferInfo;
    lightWrite.pImageInfo = nullptr;
    lightWrite.pTexelBufferView = nullptr;

    VkWriteDescriptorSet albedoWrite{};
    albedoWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    albedoWrite.dstSet = m_descriptorSetsVk.lightingSets[i];
    albedoWrite.dstBinding = 1;
    albedoWrite.dstArrayElement = 0;
    albedoWrite.descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // is this correct?
    albedoWrite.descriptorCount = 1;
    albedoWrite.pBufferInfo = nullptr;
    albedoWrite.pImageInfo = &albedoImageInfo;
    albedoWrite.pTexelBufferView = nullptr;

    VkWriteDescriptorSet depthWrite{};
    depthWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    depthWrite.dstSet = m_descriptorSetsVk.lightingSets[i];
    depthWrite.dstBinding = 2;
    depthWrite.dstArrayElement = 0;
    depthWrite.descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // is this correct?
    depthWrite.descriptorCount = 1;
    depthWrite.pBufferInfo = nullptr;
    depthWrite.pImageInfo = &depthImageInfo;
    depthWrite.pTexelBufferView = nullptr;

    VkWriteDescriptorSet normalWrite{};
    normalWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    normalWrite.dstSet = m_descriptorSetsVk.lightingSets[i];
    normalWrite.dstBinding = 3;
    normalWrite.dstArrayElement = 0;
    normalWrite.descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // is this correct?
    normalWrite.descriptorCount = 1;
    normalWrite.pBufferInfo = nullptr;
    normalWrite.pImageInfo = &normalImageInfo;
    normalWrite.pTexelBufferView = nullptr;

    std::vector<VkWriteDescriptorSet> writes{lightWrite, albedoWrite,
                                             depthWrite, normalWrite};

    vkUpdateDescriptorSets(device->GetDevice(), writes.size(), writes.data(), 0,
                           nullptr);
  };
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
  m_imagesVk.lightImage.Destroy();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
