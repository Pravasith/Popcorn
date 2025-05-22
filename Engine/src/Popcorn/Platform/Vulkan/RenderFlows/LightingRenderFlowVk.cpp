#include "RenderFlows/LightingRenderFlowVk.h"
#include "AttachmentVk.h"
#include "CommonVk.h"
#include "ContextVk.h"
#include "DescriptorLayoutsVk.h"
#include "DescriptorPoolsVk.h"
#include "FramebufferVk.h"
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

  std::vector<VkFormat> lightCandidates = {VK_FORMAT_R16G16B16A16_SFLOAT,
                                           VK_FORMAT_R8G8B8A8_UNORM};

  VkFormat lightFormat =
      ImageVk::FindSupportedFormat(lightCandidates, VK_IMAGE_TILING_OPTIMAL,
                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
  //
  // --- Create Images ---------------------------------------------------------
  VkImageCreateInfo lightImageInfo;
  ImageVk::GetDefaultImageCreateInfo(lightImageInfo, swapchainExtent.width,
                                     swapchainExtent.height, lightFormat);
  lightImageInfo.format = lightFormat;
  lightImageInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VmaAllocationCreateInfo lightImageAlloc{.usage = VMA_MEMORY_USAGE_AUTO};

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    ImageVk &lightImageRef = m_imagesVk.lightImages[i];
    lightImageRef.CreateVmaImage(lightImageInfo, lightImageAlloc);

    //
    // --- Image views
    // -----------------------------------------------------------
    VkImageViewCreateInfo lightImageViewInfo{};
    ImageVk::GetDefaultImageViewCreateInfo(
        lightImageViewInfo, lightImageRef.GetVkImage(), lightFormat);
    lightImageViewInfo.format = lightFormat;
    lightImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    lightImageRef.CreateImageView(lightImageViewInfo);

    //
    // --- Attachments
    // -----------------------------------------------------------
    VkAttachmentDescription lightImageAttachment{};
    AttachmentVk::GetDefaultAttachmentDescription(lightImageAttachment);
    lightImageAttachment.format = lightFormat;
    lightImageAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    lightImageAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    m_attachmentsVk.lightAttachments[i].SetImageVk(&lightImageRef);
    m_attachmentsVk.lightAttachments[i].SetAttachmentDescription(
        lightImageAttachment);
  }
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
  constexpr uint32_t arbitrarilyChosenFrameIndex = 0;

  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription attachments[]{
      m_attachmentsVk.lightAttachments[arbitrarilyChosenFrameIndex]
          .GetAttachmentDescription()};

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
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
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
void LightingRenderFlowVk::CreateFramebuffers() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

    std::vector<VkImageView> attachments{
        m_imagesVk.lightImages[i].GetVkImageView()};

    VkFramebufferCreateInfo createInfo{};
    FramebufferVk::GetDefaultFramebufferState(createInfo);

    createInfo.renderPass = m_renderPass.GetVkRenderPass();
    createInfo.pAttachments = attachments.data();
    createInfo.attachmentCount = attachments.size();
    createInfo.width = swapchainExtent.width;
    createInfo.height = swapchainExtent.height;
    createInfo.layers = 1;

    FramebufferVk::CreateVkFramebuffer(ContextVk::Device()->GetDevice(),
                                       createInfo, m_framebuffers[i]);
  }
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
    albedoImageInfo.imageView =
        m_dependencyImages.albedoImages[i].GetVkImageView();
    albedoImageInfo.sampler = s_samplersVk.frameSampler.GetVkSampler();
    albedoImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkDescriptorImageInfo depthImageInfo{};
    depthImageInfo.imageView =
        m_dependencyImages.depthImages[i].GetVkImageView();
    depthImageInfo.sampler = s_samplersVk.frameSampler.GetVkSampler();
    depthImageInfo.imageLayout =
        m_dependencyImages.depthImages[i].FormatHasStencilComponent()
            ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
            : VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

    VkDescriptorImageInfo normalImageInfo{};
    normalImageInfo.imageView =
        m_dependencyImages.normalImages[i].GetVkImageView();
    normalImageInfo.sampler = s_samplersVk.frameSampler.GetVkSampler();
    normalImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet lightWrite{};
    lightWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    lightWrite.dstSet = m_descriptorSetsVk.lightingSets[i];
    lightWrite.dstBinding = 0;
    lightWrite.dstArrayElement = 0;
    lightWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    lightWrite.descriptorCount = 1;
    lightWrite.pBufferInfo = &lightBufferInfo;
    lightWrite.pImageInfo = nullptr;
    lightWrite.pTexelBufferView = nullptr;

    VkWriteDescriptorSet albedoWrite{};
    albedoWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    albedoWrite.dstSet = m_descriptorSetsVk.lightingSets[i];
    albedoWrite.dstBinding = 1;
    albedoWrite.dstArrayElement = 0;
    albedoWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    albedoWrite.descriptorCount = 1;
    albedoWrite.pBufferInfo = nullptr;
    albedoWrite.pImageInfo = &albedoImageInfo;
    albedoWrite.pTexelBufferView = nullptr;

    VkWriteDescriptorSet depthWrite{};
    depthWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    depthWrite.dstSet = m_descriptorSetsVk.lightingSets[i];
    depthWrite.dstBinding = 2;
    depthWrite.dstArrayElement = 0;
    depthWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    depthWrite.descriptorCount = 1;
    depthWrite.pBufferInfo = nullptr;
    depthWrite.pImageInfo = &depthImageInfo;
    depthWrite.pTexelBufferView = nullptr;

    VkWriteDescriptorSet normalWrite{};
    normalWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    normalWrite.dstSet = m_descriptorSetsVk.lightingSets[i];
    normalWrite.dstBinding = 3;
    normalWrite.dstArrayElement = 0;
    normalWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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
void LightingRenderFlowVk::DestroyFramebuffers() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    if (m_framebuffers[i] != VK_NULL_HANDLE) {
      FramebufferVk::DestroyVkFramebuffer(ContextVk::Device()->GetDevice(),
                                          m_framebuffers[i]);
      m_framebuffers[i] = VK_NULL_HANDLE;
    }
  }
};

void LightingRenderFlowVk::DestroyRenderPass() {
  if (m_renderPass.GetVkRenderPass() != VK_NULL_HANDLE) {
    m_renderPass.Destroy(ContextVk::Device()->GetDevice());
  }
};

void LightingRenderFlowVk::DestroyAttachments() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    m_imagesVk.lightImages[i].Destroy();
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
