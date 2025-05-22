#include "RenderFlows/CompositeRenderFlowVk.h"
#include "AttachmentVk.h"
#include "CommonVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
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
  const auto &device = ContextVk::Device()->GetDevice();

  //
  // Note: Swapchain images & views are already created in
  // ContextVk::InitVulkan()

  auto &swapchainImages = swapchain->GetSwapchainImages();
  auto &swapchainImageViews = swapchain->GetSwapchainImageViews();
  const auto &format = swapchain->GetSwapchainImageFormat();

  m_imagesVk.swapchainImages.resize(swapchainImages.size());
  m_attachmentsVk.presentAttachments.resize(swapchainImages.size());

  for (size_t i = 0; i < swapchainImages.size(); ++i) {
    //
    // --- Images -------------------------------------------------------------
    m_imagesVk.swapchainImages[i].SetSwapchainImageData(
        swapchainImages[i], swapchainImageViews[i], format);

    //
    // --- Attachments ---------------------------------------------------------
    VkAttachmentDescription presentImageAttachment{};
    AttachmentVk::GetDefaultAttachmentDescription(presentImageAttachment);
    presentImageAttachment.format = format;
    presentImageAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    presentImageAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    m_attachmentsVk.presentAttachments[i].SetImageVk(
        &m_imagesVk.swapchainImages[i]);
    m_attachmentsVk.presentAttachments[i].SetAttachmentDescription(
        presentImageAttachment);
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
void CompositeRenderFlowVk::CreateRenderPass() {
  constexpr uint32_t arbitrarilyChosenSwapchainImageIndex = 0;

  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription attachments[]{
      m_attachmentsVk.presentAttachments[arbitrarilyChosenSwapchainImageIndex]
          .GetAttachmentDescription()};

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
void CompositeRenderFlowVk::CreateFramebuffers() {
  const auto &device = ContextVk::Device()->GetDevice();
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();
  auto &swapchainImgViews = ContextVk::Swapchain()->GetSwapchainImageViews();

  m_framebuffers.resize(swapchainImgViews.size());

  for (size_t i = 0; i < swapchainImgViews.size(); ++i) {
    VkImageView attachments[] = {swapchainImgViews[i]};

    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = m_renderPass.GetVkRenderPass();
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = attachments;
    createInfo.width = swapchainExtent.width;
    createInfo.height = swapchainExtent.height;
    createInfo.layers = 1;
    createInfo.pNext = VK_NULL_HANDLE;

    if (vkCreateFramebuffer(device, &createInfo, nullptr, &m_framebuffers[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create swapchain framebuffer!");
    }
  };
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
  auto *device = ContextVk::Device();
  auto *memory = ContextVk::Memory();

  VkPhysicalDeviceProperties properties{};
  device->GetPhysicalDeviceProperties(properties);

  VkDescriptorSetLayout &compositeLayout =
      ContextVk::DescriptorLayouts()->GetLayout<DescriptorSets::PresentSet>();

  std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> compositeLayouts{};
  std::fill(compositeLayouts.begin(), compositeLayouts.end(), compositeLayout);

  DPoolVk &compositePool =
      pools->GetPool<DescriptorPools::CompositePool>(MAX_FRAMES_IN_FLIGHT);

  // Creates multiple sets (from Count template param)
  m_descriptorSetsVk.presentSets =
      compositePool.AllocateDescriptorSets<DescriptorSets::PresentSet,
                                           MAX_FRAMES_IN_FLIGHT>(
          ContextVk::Device()->GetDevice(), compositeLayouts);

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    VkDescriptorImageInfo lightImageInfo{};
    lightImageInfo.imageView =
        m_dependencyImages.lightImages[i].GetVkImageView();
    lightImageInfo.sampler = s_samplersVk.frameSampler.GetVkSampler();
    lightImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet lightWrite{};
    lightWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    lightWrite.dstSet = m_descriptorSetsVk.presentSets[i];
    lightWrite.dstBinding = 0;
    lightWrite.dstArrayElement = 0;
    lightWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    lightWrite.descriptorCount = 1;
    lightWrite.pBufferInfo = nullptr;
    lightWrite.pImageInfo = &lightImageInfo;
    lightWrite.pTexelBufferView = nullptr;

    std::vector<VkWriteDescriptorSet> writes{lightWrite};

    vkUpdateDescriptorSets(device->GetDevice(), writes.size(), writes.data(), 0,
                           nullptr);
  }
};

//
//
//
//
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
//
void CompositeRenderFlowVk::DestroyFramebuffers() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    if (m_framebuffers[i] != VK_NULL_HANDLE) {
      FramebufferVk::DestroyVkFramebuffer(ContextVk::Device()->GetDevice(),
                                          m_framebuffers[i]);
      m_framebuffers[i] = VK_NULL_HANDLE;
    }
  }
};

void CompositeRenderFlowVk::DestroyRenderPass() {
  if (m_renderPass.GetVkRenderPass() != VK_NULL_HANDLE) {
    m_renderPass.Destroy(ContextVk::Device()->GetDevice());
  }
};

void CompositeRenderFlowVk::DestroyAttachments() {
  uint32_t swapchainImageCount =
      ContextVk::Swapchain()->GetSwapchainImages().size();

  for (size_t i = 0; i < swapchainImageCount; ++i) {
    m_imagesVk.swapchainImages[i].Destroy();
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
