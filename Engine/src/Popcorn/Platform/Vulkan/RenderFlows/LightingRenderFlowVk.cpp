#include "RenderFlows/LightingRenderFlowVk.h"
#include "AttachmentVk.h"
#include "BarrierVk.h"
#include "BufferObjects.h"
#include "CommonVk.h"
#include "ContextVk.h"
#include "DescriptorLayoutsVk.h"
#include "DescriptorPoolsVk.h"
#include "FramebufferVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
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
void LightingRenderFlowVk::CreateAttachments() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  std::vector<VkFormat> lightCandidates = {VK_FORMAT_R16G16B16A16_SFLOAT,
                                           VK_FORMAT_R8G8B8A8_UNORM};

  VkFormat lightFormat =
      ImageVk::FindSupportedFormat(lightCandidates, VK_IMAGE_TILING_OPTIMAL,
                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
  //
  // --- Create Images ---------------------------------------------------------
  VkImageCreateInfo lightImageInfo{};
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
    lightImageAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    lightImageAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    lightImageAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

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
// --- IMAGE BARRIERS  ---------------------------------------------------------
// --- IMAGE BARRIERS  ---------------------------------------------------------
// --- IMAGE BARRIERS  ---------------------------------------------------------
//
void LightingRenderFlowVk::CreateImageBarriers() {
  // For AFTER the current renderpass and BEFORE the next renderpass
  // Color/depth attachment -> shader read format
  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    ImageBarrierVk<LayoutTransitions::ColorAttachmentToShaderRead>
        &lightBarrier = m_imageBarriers.lightBarriers[i];

    lightBarrier.Init(&m_imagesVk.lightImages[i]);
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
  dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
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

//
//
//
//
//
// --- CREATE COMMAND BUFFERS --------------------------------------------------
// --- CREATE COMMAND BUFFERS --------------------------------------------------
// --- CREATE COMMAND BUFFERS --------------------------------------------------
//
void LightingRenderFlowVk::CreateCommandBuffers() {
  auto *cmdPool = ContextVk::CommandPool();

  VkCommandBufferAllocateInfo allocInfo{};
  cmdPool->GetDefaultCommandBufferAllocInfo(allocInfo);
  allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

  cmdPool->AllocCommandBuffers(allocInfo, m_commandBuffers.data());
};

void LightingRenderFlowVk::AllocDescriptorsLocal() {
  auto *pools = ContextVk::DescriptorPools();
  // VkPhysicalDeviceProperties properties{};
  // device->GetPhysicalDeviceProperties(properties);

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
};

void LightingRenderFlowVk::UpdateDescriptorSetsLocal() {
  auto *memory = ContextVk::Memory();
  auto *device = ContextVk::Device();

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    VkDescriptorBufferInfo lightBufferInfo{};
    lightBufferInfo.buffer = memory->GetSsboSet(i);
    lightBufferInfo.offset = memory->GetBufferViews().lightsSsbo.offset;
    lightBufferInfo.range = memory->GetBufferViews().lightsSsbo.alignedSize;

    // DescriptorLayoutsVk::GetDescriptorBufferRange<
    // DescriptorSets::LightingSet>(properties.limits);

    VkDescriptorImageInfo albedoImageInfo{};
    albedoImageInfo.imageView =
        m_dependencyImages.albedoImages[i].GetVkImageView();
    albedoImageInfo.sampler = s_samplersVk.colorSampler.GetVkSampler();
    albedoImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkDescriptorImageInfo depthImageInfo{};
    depthImageInfo.imageView =
        m_dependencyImages.depthImages[i].GetVkImageView();
    depthImageInfo.sampler = s_samplersVk.depthSampler.GetVkSampler();
    depthImageInfo.imageLayout =
        m_dependencyImages.depthImages[i].FormatHasStencilComponent()
            ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
            : VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

    VkDescriptorImageInfo normalImageInfo{};
    normalImageInfo.imageView =
        m_dependencyImages.normalImages[i].GetVkImageView();
    normalImageInfo.sampler = s_samplersVk.colorSampler.GetVkSampler();
    normalImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkDescriptorImageInfo roughnessMetallicImageInfo{};
    roughnessMetallicImageInfo.imageView =
        m_dependencyImages.roughnessMetallicImages[i].GetVkImageView();
    roughnessMetallicImageInfo.sampler =
        s_samplersVk.colorSampler.GetVkSampler();
    roughnessMetallicImageInfo.imageLayout =
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    std::vector<VkWriteDescriptorSet> writes{};
    writes.reserve(5);

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
    writes.emplace_back(lightWrite);

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
    writes.emplace_back(albedoWrite);

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
    writes.emplace_back(depthWrite);

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
    writes.emplace_back(normalWrite);

    VkWriteDescriptorSet roughnessMetallicWrite{};
    roughnessMetallicWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    roughnessMetallicWrite.dstSet = m_descriptorSetsVk.lightingSets[i];
    roughnessMetallicWrite.dstBinding = 4;
    roughnessMetallicWrite.dstArrayElement = 0;
    roughnessMetallicWrite.descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    roughnessMetallicWrite.descriptorCount = 1;
    roughnessMetallicWrite.pBufferInfo = nullptr;
    roughnessMetallicWrite.pImageInfo = &roughnessMetallicImageInfo;
    roughnessMetallicWrite.pTexelBufferView = nullptr;
    writes.emplace_back(roughnessMetallicWrite);

    vkUpdateDescriptorSets(device->GetDevice(), writes.size(), writes.data(), 0,
                           nullptr);
  };
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
void LightingRenderFlowVk::CreatePipelines() {
  using namespace BufferDefs;
  BufferDefs::Layout layout{};
  // for a triangle
  layout.Set<AttrTypes::Float2>();
  m_lightingPipelineVk.Create(layout, m_renderPass.GetVkRenderPass());
};

void LightingRenderFlowVk::DestroyPipelines() {
  m_lightingPipelineVk.Destroy();
};

//
//
//
//
// --- RECREATE RESOURCES ------------------------------------------------------
// --- RECREATE RESOURCES ------------------------------------------------------
// --- RECREATE RESOURCES ------------------------------------------------------
//
void LightingRenderFlowVk::OnSwapchainInvalidCb() {
  DestroyFramebuffers();
  DestroyAttachments();

  CreateAttachments();
  CreateImageBarriers();
  CreateFramebuffers();

  UpdateDescriptorSetsLocal();
};

//
//
//
//
// --- PAINT -------------------------------------------------------------------
// --- PAINT -------------------------------------------------------------------
// --- PAINT -------------------------------------------------------------------
void LightingRenderFlowVk::RecordCommandBuffer(const uint32_t frameIndex,
                                               const uint32_t currentFrame) {
  // TODO: Optimize draw loop - heap allocations -> stack
  auto &cmdBfr = m_commandBuffers[currentFrame];
  auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();
  auto *deviceMemory =
      ContextVk::Memory(); // Heap allocated!! this is singleton

  auto &bufferViews = deviceMemory->GetBufferViews();
  auto &bufferOffsets = deviceMemory->GetBufferOffsets();

  VkDescriptorSet &cameraSet = s_commonDescriptorSets.cameraSets[currentFrame];
  VkDescriptorSet &lightingSet = m_descriptorSetsVk.lightingSets[currentFrame];

  std::array<VkDescriptorSet, 1> cameraSets{
      s_commonDescriptorSets.cameraSets[currentFrame],
  };
  std::array<VkDescriptorSet, 1> lightingSets{
      m_descriptorSetsVk.lightingSets[currentFrame],
  };

  // Reset and begin recording
  vkResetCommandBuffer(cmdBfr, 0);
  ContextVk::CommandPool()->BeginCommandBuffer(cmdBfr);

  VkClearValue clearColor = {{0.0f, 0.0f, 0.0f, 1.0f}};
  std::vector<VkClearValue> clearValues{clearColor};

  // Render pass begin
  VkRenderPassBeginInfo renderPassBeginInfo{};
  RenderPassVk::GetDefaultCmdBeginRenderPassInfo(
      m_framebuffers[currentFrame], swapchainExtent,
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
  m_lightingPipelineVk.BindPipeline(cmdBfr);

  // Hardcoding 0 for camera index for now
  // TODO: Make it dynamic later
  uint32_t cameraOffset = bufferOffsets.camerasOffsets[0];

  std::array<VkDescriptorSet, 2> allSets = {cameraSet, lightingSet};
  std::array<uint32_t, 1> dynamicOffsets = {cameraOffset};

  // Binding 2 sets in one go. Same as doing separately (commented code
  // below)
  vkCmdBindDescriptorSets(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          m_lightingPipelineVk.GetVkPipelineLayout(), 0,
                          allSets.size(), allSets.data(), dynamicOffsets.size(),
                          dynamicOffsets.data());

  // // Descriptor set 0 - Camera
  // vkCmdBindDescriptorSets(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS,
  //                         m_lightingPipelineVk.GetVkPipelineLayout(), 0,
  //                         cameraSets.size(), cameraSets.data(), 1,
  //                         &cameraOffset);
  //
  // // Descriptor set 1 - Lighting(ssbo + gbuffer textures)
  // vkCmdBindDescriptorSets(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS,
  //                         m_lightingPipelineVk.GetVkPipelineLayout(), 1,
  //                         lightingSets.size(), lightingSets.data(), 0,
  //                         nullptr);

  // Full screen triangle
  vkCmdDraw(cmdBfr, 3, 1, 0, 0);

  m_renderPass.EndRenderPass(cmdBfr);

  //
  // --- Transition image layouts for next pass --------------------------------
  ImageBarrierVk<LayoutTransitions::ColorAttachmentToShaderRead> &lightBarrier =
      m_imageBarriers.lightBarriers[currentFrame];
  lightBarrier.RecordBarrierCommand(cmdBfr);

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
