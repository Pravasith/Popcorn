#include "CompositeRenderFlowVk.h"
#include "AttachmentVk.h"
#include "BarrierVk.h"
#include "CommonVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
#include "PipelineUtilsVk.h"
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
    presentImageAttachment.finalLayout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    presentImageAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    presentImageAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

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
// --- IMAGE BARRIERS  ---------------------------------------------------------
// --- IMAGE BARRIERS  ---------------------------------------------------------
// --- IMAGE BARRIERS  ---------------------------------------------------------
//
void CompositeRenderFlowVk::CreateImageBarriers() {
  auto &swapchainImages = ContextVk::Swapchain()->GetSwapchainImages();
  m_imageBarriers.presentBarriers.resize(swapchainImages.size());

  // For AFTER the current renderpass and BEFORE the next renderpass
  // Color/depth attachment -> shader read format
  for (int i = 0; i < swapchainImages.size(); ++i) {
    ImageBarrierVk<LayoutTransitions::ColorAttachmentToPresentSrc>
        &presentBarrier = m_imageBarriers.presentBarriers[i];

    presentBarrier.Init(&m_imagesVk.swapchainImages[i]);
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
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
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
}

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
}

//
//
//
//
//
// --- CREATE COMMAND BUFFERS --------------------------------------------------
// --- CREATE COMMAND BUFFERS --------------------------------------------------
// --- CREATE COMMAND BUFFERS --------------------------------------------------
//
void CompositeRenderFlowVk::CreateCommandBuffers() {
  auto *cmdPool = ContextVk::CommandPool();

  VkCommandBufferAllocateInfo allocInfo{};
  cmdPool->GetDefaultCommandBufferAllocInfo(allocInfo);
  allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

  cmdPool->AllocCommandBuffers(allocInfo, m_commandBuffers.data());
}

//
//
//
//
//
// --- CREATE DESCRIPTORS ------------------------------------------------------
// --- CREATE DESCRIPTORS ------------------------------------------------------
// --- CREATE DESCRIPTORS ------------------------------------------------------
//
void CompositeRenderFlowVk::AllocDescriptorsLocal() {
  auto *pools = ContextVk::DescriptorPools();
  // auto *memory = ContextVk::Memory();

  // VkPhysicalDeviceProperties properties{};
  // device->GetPhysicalDeviceProperties(properties);

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
}

void CompositeRenderFlowVk::UpdateDescriptorSetsLocal() {
  auto *device = ContextVk::Device();

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    VkDescriptorImageInfo lightImageInfo{};
    lightImageInfo.imageView =
        m_dependencyImages.lightImages[i].GetVkImageView();
    lightImageInfo.sampler = s_samplersVk.colorSampler.GetVkSampler();
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
}

//
//
//
//
//
// --- CREATE PIPELINES --------------------------------------------------------
// --- CREATE PIPELINES --------------------------------------------------------
// --- CREATE PIPELINES --------------------------------------------------------
//
void CompositeRenderFlowVk::CreatePipelines() {
  using namespace BufferDefs;
  BufferDefs::Layout layout{};
  // for a triangle
  layout.Set<AttrTypes::Float2>();
  m_compositePipelineVk.Create(layout, m_renderPass.GetVkRenderPass());
}

void CompositeRenderFlowVk::DestroyPipelines() {
  m_compositePipelineVk.Destroy();
}

//
//
//
//
// --- RECREATE RESOURCES ------------------------------------------------------
// --- RECREATE RESOURCES ------------------------------------------------------
// --- RECREATE RESOURCES ------------------------------------------------------
//
void CompositeRenderFlowVk::OnSwapchainInvalidCb() {
  DestroyPipelines();
  DestroyFramebuffers();
  DestroyRenderPass();
  DestroyAttachments();

  ContextVk::Swapchain()->RecreateSwapchainAndVkSwapchain();

  CreateAttachments();
  CreateImageBarriers();
  CreateRenderPass();
  CreateFramebuffers();

  UpdateDescriptorSetsLocal();
  CreatePipelines();
}

//
//
//
//
// --- PAINT -------------------------------------------------------------------
// --- PAINT -------------------------------------------------------------------
// --- PAINT -------------------------------------------------------------------
void CompositeRenderFlowVk::RecordCommandBuffer(const uint32_t frameIndex,
                                                const uint32_t currentFrame) {
  // TODO: Optimize draw loop - heap allocations -> stack
  auto &cmdBfr = m_commandBuffers[currentFrame];

  auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();
  auto *deviceMemory =
      ContextVk::Memory(); // Heap allocated!! this is singleton

  auto &bufferViews = deviceMemory->GetBufferViews();
  auto &bufferOffsets = deviceMemory->GetBufferOffsets();

  std::array<VkDescriptorSet, 1> compositeSets{
      m_descriptorSetsVk.presentSets[currentFrame]};

  // Reset and begin recording
  vkResetCommandBuffer(cmdBfr, 0);
  ContextVk::CommandPool()->BeginCommandBuffer(cmdBfr);

  VkClearValue clearColor = {{1.0f, 0.0f, 1.0f, 1.0f}};
  std::vector<VkClearValue> clearValues{clearColor};

  // Render pass begin
  VkRenderPassBeginInfo renderPassBeginInfo{};
  RenderPassVk::GetDefaultCmdBeginRenderPassInfo(
      m_framebuffers[frameIndex], swapchainExtent,
      m_renderPass.GetVkRenderPass(), clearValues, renderPassBeginInfo);

  m_renderPass.BeginRenderPass(cmdBfr, renderPassBeginInfo);

  // Set viewport and scissor
  VkViewport viewport{};
  VkRect2D scissor{};
  PipelineUtilsVk::GetDefaultViewportAndScissorState(viewport, scissor,
                                                     swapchainExtent);
  vkCmdSetViewport(cmdBfr, 0, 1, &viewport);
  vkCmdSetScissor(cmdBfr, 0, 1, &scissor);

#ifdef PC_DEBUG
  static int drawCommandCount = 0;
#endif

  //
  // --- Paint :D --------------------------------------------------------------
  m_compositePipelineVk.BindPipeline(cmdBfr);

  // Descriptor set 0 - Present set
  vkCmdBindDescriptorSets(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          m_compositePipelineVk.GetVkPipelineLayout(), 0,
                          compositeSets.size(), compositeSets.data(), 0,
                          nullptr);

  // Full screen triangle
  vkCmdDraw(cmdBfr, 3, 1, 0, 0);

  m_renderPass.EndRenderPass(cmdBfr);

  //
  // --- Transition image layouts for next pass --------------------------------
  ImageBarrierVk<LayoutTransitions::ColorAttachmentToPresentSrc>
      &presentBarrier = m_imageBarriers.presentBarriers[frameIndex];
  presentBarrier.RecordBarrierCommand(cmdBfr);

  //
  // --- End command buffer ----------------------------------------------------
  ContextVk::CommandPool()->EndCommandBuffer(cmdBfr);
}

//
//
//
//
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
//
void CompositeRenderFlowVk::DestroyFramebuffers() {
  for (size_t i = 0; i < m_framebuffers.size(); ++i) {
    if (m_framebuffers[i] != VK_NULL_HANDLE) {
      FramebufferVk::DestroyVkFramebuffer(ContextVk::Device()->GetDevice(),
                                          m_framebuffers[i]);
      m_framebuffers[i] = VK_NULL_HANDLE;
    }
  }

  m_framebuffers.clear();
}

void CompositeRenderFlowVk::DestroyRenderPass() {
  if (m_renderPass.GetVkRenderPass() != VK_NULL_HANDLE) {
    m_renderPass.Destroy(ContextVk::Device()->GetDevice());
  }
}

void CompositeRenderFlowVk::DestroyAttachments() {
  uint32_t swapchainImageCount =
      ContextVk::Swapchain()->GetSwapchainImages().size();

  for (size_t i = 0; i < swapchainImageCount; ++i) {
    m_imagesVk.swapchainImages[i].Destroy();
  }
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
