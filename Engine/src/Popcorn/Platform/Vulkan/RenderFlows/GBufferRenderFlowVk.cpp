#include "RenderFlows/GBufferRenderFlowVk.h"
#include "AttachmentVk.h"
#include "BufferObjectsVk.h"
#include "CommonVk.h"
#include "ContextVk.h"
#include "DescriptorLayoutsVk.h"
#include "DescriptorPoolsVk.h"
#include "FramebufferVk.h"
#include "ImageVk.h"
#include "MaterialTypes.h"
#include "Memory/MemoryVk.h"
#include "Mesh.h"
#include "PipelineUtilsVk.h"
#include "Popcorn/Loaders/LoadersDefs.h"
#include "RenderPassVk.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// --- CREATE ATTACHMENTS ------------------------------------------------------
// --- CREATE ATTACHMENTS ------------------------------------------------------
// --- CREATE ATTACHMENTS ------------------------------------------------------
//
void GBufferRenderFlowVk::CreateAttachments() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  const auto &width = swapchainExtent.width;
  const auto &height = swapchainExtent.height;

  std::vector<VkFormat> albedoCandidates = {VK_FORMAT_R8G8B8A8_UNORM,
                                            VK_FORMAT_B8G8R8A8_UNORM};
  std::vector<VkFormat> depthCandidates = {VK_FORMAT_D32_SFLOAT,
                                           VK_FORMAT_D32_SFLOAT_S8_UINT,
                                           VK_FORMAT_D24_UNORM_S8_UINT};
  std::vector<VkFormat> normalCandidates = {
      VK_FORMAT_R16G16B16A16_SFLOAT, // high precision normals
      VK_FORMAT_R8G8B8A8_UNORM       // Fallback (low precision)
  };
  std::vector<VkFormat> roughnessMetallicCandidates = {
      VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM};

  VkFormat albedoFormat =
      ImageVk::FindSupportedFormat(albedoCandidates, VK_IMAGE_TILING_OPTIMAL,
                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
  VkFormat depthFormat = ImageVk::FindSupportedFormat(
      depthCandidates, VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  VkFormat normalFormat =
      ImageVk::FindSupportedFormat(normalCandidates, VK_IMAGE_TILING_OPTIMAL,
                                   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
  VkFormat roughnessMetallicFormat = ImageVk::FindSupportedFormat(
      roughnessMetallicCandidates, VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);

  //
  //
  // --- Create g-buffer images ------------------------------------------------
  VkImageCreateInfo albedoImageInfo{};
  ImageVk::GetDefaultImageCreateInfo(albedoImageInfo, width, height,
                                     albedoFormat);
  albedoImageInfo.format = albedoFormat;
  albedoImageInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VkImageCreateInfo depthImageInfo{};
  ImageVk::GetDefaultImageCreateInfo(depthImageInfo, width, height,
                                     depthFormat);
  depthImageInfo.format = depthFormat;
  depthImageInfo.usage =
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VkImageCreateInfo normalImageInfo{};
  ImageVk::GetDefaultImageCreateInfo(normalImageInfo, width, height,
                                     normalFormat);
  normalImageInfo.format = normalFormat;
  normalImageInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VkImageCreateInfo roughnessMetallicImageInfo{};
  ImageVk::GetDefaultImageCreateInfo(roughnessMetallicImageInfo, width, height,
                                     roughnessMetallicFormat);
  roughnessMetallicImageInfo.format = roughnessMetallicFormat;
  roughnessMetallicImageInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VmaAllocationCreateInfo albedoAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo depthAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo normalAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo roughnessMetallicAlloc{.usage =
                                                     VMA_MEMORY_USAGE_AUTO};

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    ImageVk &albedoImage = m_imagesVk.albedoImages[i];
    ImageVk &depthImage = m_imagesVk.depthImages[i];
    ImageVk &normalImage = m_imagesVk.normalImages[i];
    ImageVk &roughnessMetallicImage = m_imagesVk.roughnessMetallicImages[i];

    albedoImage.CreateVmaImage(albedoImageInfo, albedoAlloc);
    depthImage.CreateVmaImage(depthImageInfo, depthAlloc);
    normalImage.CreateVmaImage(normalImageInfo, normalAlloc);
    roughnessMetallicImage.CreateVmaImage(roughnessMetallicImageInfo,
                                          roughnessMetallicAlloc);

    //
    //
    // --- Create g-buffer image-views -----------------------------------------
    VkImageViewCreateInfo albedoViewInfo{};
    ImageVk::GetDefaultImageViewCreateInfo(
        albedoViewInfo, albedoImage.GetVkImage(), albedoFormat);
    albedoViewInfo.format = albedoFormat;
    albedoViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    VkImageViewCreateInfo depthViewInfo{};
    ImageVk::GetDefaultImageViewCreateInfo(
        depthViewInfo, depthImage.GetVkImage(), depthFormat);
    depthViewInfo.format = depthFormat;
    depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (depthImage.FormatHasStencilComponent()) {
      depthViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkImageViewCreateInfo normalViewInfo{};
    ImageVk::GetDefaultImageViewCreateInfo(
        normalViewInfo, normalImage.GetVkImage(), normalFormat);
    normalViewInfo.format = normalFormat;
    normalViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    VkImageViewCreateInfo roughnessMetallicViewInfo{};
    ImageVk::GetDefaultImageViewCreateInfo(roughnessMetallicViewInfo,
                                           roughnessMetallicImage.GetVkImage(),
                                           roughnessMetallicFormat);
    roughnessMetallicViewInfo.format = roughnessMetallicFormat;
    roughnessMetallicViewInfo.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_COLOR_BIT;

    albedoImage.CreateImageView(albedoViewInfo);
    depthImage.CreateImageView(depthViewInfo);
    normalImage.CreateImageView(normalViewInfo);
    roughnessMetallicImage.CreateImageView(roughnessMetallicViewInfo);

    //
    //
    // --- Create g-buffer attachments -----------------------------------------
    VkAttachmentDescription albedoAttachment{};
    AttachmentVk::GetDefaultAttachmentDescription(albedoAttachment);
    albedoAttachment.format = albedoFormat;
    albedoAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    albedoAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    albedoAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    albedoAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkAttachmentDescription depthAttachment{};
    AttachmentVk::GetDefaultAttachmentDescription(depthAttachment);
    depthAttachment.format = depthFormat;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout =
        depthImage.FormatHasStencilComponent()
            ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
            : VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = depthImage.FormatHasStencilComponent()
                                        ? VK_ATTACHMENT_LOAD_OP_DONT_CARE
                                        : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = depthImage.FormatHasStencilComponent()
                                         ? VK_ATTACHMENT_STORE_OP_STORE
                                         : VK_ATTACHMENT_STORE_OP_DONT_CARE;

    VkAttachmentDescription normalAttachment{};
    AttachmentVk::GetDefaultAttachmentDescription(normalAttachment);
    normalAttachment.format = normalFormat;
    normalAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    normalAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    normalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    normalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkAttachmentDescription roughnessMetallicAttachment{};
    AttachmentVk::GetDefaultAttachmentDescription(roughnessMetallicAttachment);
    roughnessMetallicAttachment.format = roughnessMetallicFormat;
    roughnessMetallicAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    roughnessMetallicAttachment.finalLayout =
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    roughnessMetallicAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    roughnessMetallicAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    m_attachmentsVk.albedoAttachments[i].SetImageVk(&albedoImage);
    m_attachmentsVk.depthAttachments[i].SetImageVk(&depthImage);
    m_attachmentsVk.normalAttachments[i].SetImageVk(&normalImage);
    m_attachmentsVk.roughnessMetallicAttachments[i].SetImageVk(
        &roughnessMetallicImage);

    m_attachmentsVk.albedoAttachments[i].SetAttachmentDescription(
        albedoAttachment);
    m_attachmentsVk.depthAttachments[i].SetAttachmentDescription(
        depthAttachment);
    m_attachmentsVk.normalAttachments[i].SetAttachmentDescription(
        normalAttachment);
    m_attachmentsVk.roughnessMetallicAttachments[i].SetAttachmentDescription(
        roughnessMetallicAttachment);
  }
};

//
//
//
//
//
// --- CREATE RENDER PASS ------------------------------------------------------
// --- CREATE RENDER PASS ------------------------------------------------------
// --- CREATE RENDER PASS ------------------------------------------------------
//
void GBufferRenderFlowVk::CreateRenderPass() {
  constexpr uint32_t arbitrarilyChosenFrameIndex = 0;

  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription attachments[]{
      m_attachmentsVk.albedoAttachments[arbitrarilyChosenFrameIndex]
          .GetAttachmentDescription(),
      m_attachmentsVk.depthAttachments[arbitrarilyChosenFrameIndex]
          .GetAttachmentDescription(),
      m_attachmentsVk.normalAttachments[arbitrarilyChosenFrameIndex]
          .GetAttachmentDescription(),
      m_attachmentsVk.roughnessMetallicAttachments[arbitrarilyChosenFrameIndex]
          .GetAttachmentDescription()};

  bool depthHasStencilComponent =
      m_imagesVk.depthImages[arbitrarilyChosenFrameIndex]
          .FormatHasStencilComponent();

  //
  // --- Attachment references -------------------------------------------------
  VkAttachmentReference albedoRef{};
  AttachmentVk::GetAttachmentRef(albedoRef, 0);
  albedoRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthRef{};
  AttachmentVk::GetAttachmentRef(depthRef, 1);
  depthRef.layout = depthHasStencilComponent
                        ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                        : VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

  VkAttachmentReference normalRef{};
  AttachmentVk::GetAttachmentRef(normalRef, 2);
  normalRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference roughnessMetallicRef{};
  AttachmentVk::GetAttachmentRef(roughnessMetallicRef, 3);
  roughnessMetallicRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentsRefs[]{albedoRef, normalRef,
                                               roughnessMetallicRef};

  //
  // --- Create Subpasses ------------------------------------------------------
  VkSubpassDescription subpass{};
  RenderPassVk::GetDefaultSubpassDescription(subpass);
  subpass.pColorAttachments = colorAttachmentsRefs;
  subpass.pDepthStencilAttachment = &depthRef;
  subpass.colorAttachmentCount = 3;
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  VkSubpassDescription subpasses[]{subpass};

  //
  // --- Dependencies ----------------------------------------------------------
  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  //
  // We're taking into consideration the prev frame's stages for the same
  // gbuffer pass (not previous light/postfx... etc renderpasses/subpasses). The
  // depth, color writes need to finish in the prev's gbuffer pass before we
  // start writing depth/color writes in the current frame. Remember -
  // MaxFramesPerFlight number of frames run in parallel.
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  //
  // --- Renderpass ------------------------------------------------------------
  VkRenderPassCreateInfo renderPassInfo{};
  RenderPassVk::GetDefaultRenderPassCreateInfo(renderPassInfo);
  renderPassInfo.attachmentCount = 4;
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
void GBufferRenderFlowVk::CreateFramebuffers() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    std::vector<VkImageView> attachments = {
        m_imagesVk.albedoImages[i].GetVkImageView(),
        m_imagesVk.depthImages[i].GetVkImageView(),
        m_imagesVk.normalImages[i].GetVkImageView(),
        m_imagesVk.roughnessMetallicImages[i].GetVkImageView(),
    };

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
void GBufferRenderFlowVk::CreateCommandBuffers() {
  auto *cmdPool = ContextVk::CommandPool();

  VkCommandBufferAllocateInfo allocInfo{};
  cmdPool->GetDefaultCommandBufferAllocInfo(allocInfo);
  allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

  cmdPool->AllocCommandBuffers(allocInfo, m_commandBuffers.data());
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
void GBufferRenderFlowVk::AllocDescriptorsLocal() {
  auto *layouts = ContextVk::DescriptorLayouts();
  auto *pools = ContextVk::DescriptorPools();
  auto *device = ContextVk::Device();

  VkPhysicalDeviceProperties properties{};
  device->GetPhysicalDeviceProperties(properties);

  constexpr uint32_t maxFIF = MAX_FRAMES_IN_FLIGHT;

  DPoolVk &gBufferPool = pools->GetPool<DescriptorPools::GBufferPool>(
      MAX_FRAMES_IN_FLIGHT); // Creates pool if it
                             // doesn't exist

  VkDescriptorSetLayout &submeshLayout =
      layouts->GetLayout<DescriptorSets::SubmeshSet>();
  std::array<VkDescriptorSetLayout, maxFIF> submeshLayouts{};
  std::fill(submeshLayouts.begin(), submeshLayouts.end(), submeshLayout);

  VkDescriptorSetLayout &basicMatLayout =
      layouts->GetLayout<DescriptorSets::BasicMatSet>();
  std::array<VkDescriptorSetLayout, maxFIF> basicMatLayouts{};
  std::fill(basicMatLayouts.begin(), basicMatLayouts.end(), basicMatLayout);

  VkDescriptorSetLayout &pbrMatLayout =
      layouts->GetLayout<DescriptorSets::PbrMatSet>();
  std::array<VkDescriptorSetLayout, maxFIF> pbrMatLayouts{};
  std::fill(pbrMatLayouts.begin(), pbrMatLayouts.end(), pbrMatLayout);

  //
  // --- 4 sets each frame ---
  // - Submesh - Dynamic UBO
  // - BasicMat - Dynamic UBO
  // - PbrMat - Dynamic UBO
  //
  //
  // Descriptor set will be cleaned automatically when pools are destroyed
  m_descriptorSetsVk.submeshSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::SubmeshSet, maxFIF>(
          device->GetDevice(), submeshLayouts);
  m_descriptorSetsVk.basicMatSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::BasicMatSet, maxFIF>(
          device->GetDevice(), basicMatLayouts);
  m_descriptorSetsVk.pbrMatSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::PbrMatSet, maxFIF>(
          device->GetDevice(), pbrMatLayouts);
};

void GBufferRenderFlowVk::UpdateDescriptorSetsLocal() {
  auto *memory = ContextVk::Memory();
  auto *device = ContextVk::Device();

  // Bind sets with buffers
  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    VkDescriptorBufferInfo submeshBufferInfo{};
    submeshBufferInfo.buffer = memory->GetUboSet(i);
    submeshBufferInfo.offset = memory->GetBufferViews().submeshUbo.offset;
    submeshBufferInfo.range = memory->GetBufferViews().submeshUbo.alignedSize;
    // DescriptorLayoutsVk::GetDescriptorBufferRange<
    //     DescriptorSets::SubmeshSet>(properties.limits);

    VkDescriptorBufferInfo basicMatBufferInfo{};
    basicMatBufferInfo.buffer = memory->GetUboSet(i);
    basicMatBufferInfo.offset = memory->GetBufferViews().basicMatUbo.offset;
    basicMatBufferInfo.range = memory->GetBufferViews().basicMatUbo.alignedSize;
    // DescriptorLayoutsVk::GetDescriptorBufferRange<
    // DescriptorSets::BasicMatSet>(properties.limits);

    VkDescriptorBufferInfo pbrMatBufferInfo{};
    pbrMatBufferInfo.buffer = memory->GetUboSet(i);
    pbrMatBufferInfo.offset = memory->GetBufferViews().pbrMatUbo.offset;
    pbrMatBufferInfo.range = memory->GetBufferViews().pbrMatUbo.alignedSize;
    // DescriptorLayoutsVk::GetDescriptorBufferRange<
    // DescriptorSets::PbrMatSet>(properties.limits);

    // Writes
    VkWriteDescriptorSet submeshWrite{};
    submeshWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    submeshWrite.dstSet = m_descriptorSetsVk.submeshSets[i];
    submeshWrite.dstBinding = 0;
    submeshWrite.dstArrayElement = 0;
    submeshWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    submeshWrite.descriptorCount = 1;
    submeshWrite.pBufferInfo = &submeshBufferInfo;
    submeshWrite.pImageInfo = nullptr;
    submeshWrite.pTexelBufferView = nullptr;

    VkWriteDescriptorSet basicMatWrite{};
    basicMatWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    basicMatWrite.dstSet = m_descriptorSetsVk.basicMatSets[i];
    basicMatWrite.dstBinding = 0;
    basicMatWrite.dstArrayElement = 0;
    basicMatWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    basicMatWrite.descriptorCount = 1;
    basicMatWrite.pBufferInfo = &basicMatBufferInfo;
    basicMatWrite.pImageInfo = nullptr;
    basicMatWrite.pTexelBufferView = nullptr;

    VkWriteDescriptorSet pbrMatWrite{};
    pbrMatWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    pbrMatWrite.dstSet = m_descriptorSetsVk.pbrMatSets[i];
    pbrMatWrite.dstBinding = 0;
    pbrMatWrite.dstArrayElement = 0;
    pbrMatWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    pbrMatWrite.descriptorCount = 1;
    pbrMatWrite.pBufferInfo = &pbrMatBufferInfo;
    pbrMatWrite.pImageInfo = nullptr;
    pbrMatWrite.pTexelBufferView = nullptr;

    std::vector<VkWriteDescriptorSet> writes{submeshWrite, basicMatWrite,
                                             pbrMatWrite};

    vkUpdateDescriptorSets(device->GetDevice(), writes.size(), writes.data(), 0,
                           nullptr);
  };
};

//
//
//
//
//
// --- CREATE PIPELINES --------------------------------------------------------
// --- CREATE PIPELINES --------------------------------------------------------
// --- CREATE PIPELINES --------------------------------------------------------
//
void GBufferRenderFlowVk::CreatePipelines() {
  // Draws Gltf models
  m_basicMatPipelineVk.Create(GltfVertexBufferLayout,
                              m_renderPass.GetVkRenderPass());
  m_pbrMatPipelineVk.Create(GltfVertexBufferLayout,
                            m_renderPass.GetVkRenderPass());
};

void GBufferRenderFlowVk::DestroyPipelines() {
  m_pbrMatPipelineVk.Destroy();
  m_basicMatPipelineVk.Destroy();
};

//
//
//
//
// --- RECREATE RESOURCES ------------------------------------------------------
// --- RECREATE RESOURCES ------------------------------------------------------
// --- RECREATE RESOURCES ------------------------------------------------------
//
void GBufferRenderFlowVk::OnSwapchainInvalidCb() {
  DestroyFramebuffers();
  DestroyAttachments();

  CreateAttachments();
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
void GBufferRenderFlowVk::RecordCommandBuffer(const uint32_t frameIndex,
                                              const uint32_t currentFrame) {
  // TODO: Optimize draw loop - heap allocations -> stack
  auto &cmdBfr = m_commandBuffers[currentFrame];
  auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();
  auto *deviceMemory =
      ContextVk::Memory(); // Heap allocated!! this is singleton

  auto &bufferViews = deviceMemory->GetBufferViews();
  auto &bufferOffsets = deviceMemory->GetBufferOffsets();

  std::array<VkDescriptorSet, 1> cameraSets{
      s_commonDescriptorSets.cameraSets[currentFrame],
  };
  std::array<VkDescriptorSet, 1> basicMatSets{
      m_descriptorSetsVk.basicMatSets[currentFrame],
  };
  std::array<VkDescriptorSet, 1> pbrMatSets{
      m_descriptorSetsVk.pbrMatSets[currentFrame],
  };
  std::array<VkDescriptorSet, 1> submeshSets{
      m_descriptorSetsVk.submeshSets[currentFrame],
  };

  const VkBuffer &vertexBuffer = deviceMemory->GetVboVkBuffer();
  const VkBuffer &indexBuffer = deviceMemory->GetIboVkBuffer();

  VkBuffer vertexBuffers[]{vertexBuffer};
  VkDeviceSize vboOffsets[]{
      0}; // This is NOT stride, it's just the VkBuffer offset

  vkResetCommandBuffer(cmdBfr, 0);
  ContextVk::CommandPool()->BeginCommandBuffer(cmdBfr);
  VkRenderPassBeginInfo renderPassBeginInfo{};
  RenderPassVk::GetDefaultCmdBeginRenderPassInfo(
      m_framebuffers[currentFrame], swapchainExtent,
      m_renderPass.GetVkRenderPass(), renderPassBeginInfo);

  //
  // --- Begin renderpass ------------------------------------------------------
  m_renderPass.BeginRenderPass(cmdBfr, renderPassBeginInfo);

  //
  // --- Set viewport & scissor ------------------------------------------------
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
  m_basicMatPipelineVk.BindPipeline(cmdBfr);

  // Hardcoding 0 for camera index for now
  // TODO: Make it dynamic later
  uint32_t cameraOffset = bufferOffsets.camerasOffsets[0];

  // Descriptor set 0 - Camera
  vkCmdBindDescriptorSets(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          m_basicMatPipelineVk.GetVkPipelineLayout(), 0,
                          cameraSets.size(), cameraSets.data(), 1,
                          &cameraOffset);

  for (auto &[materialHash, submeshes] : s_basicMatSubmeshesMap) {
    uint32_t basicMatOffset = bufferOffsets.materialOffsets[materialHash];

    // Descriptor set 1 - Basic material
    vkCmdBindDescriptorSets(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_basicMatPipelineVk.GetVkPipelineLayout(), 1,
                            basicMatSets.size(), basicMatSets.data(), 1,
                            &basicMatOffset);

    uint32_t submeshIndex = 0;
    for (Submesh<MaterialTypes::BasicMat> *submesh : submeshes) {
      uint32_t submeshUboOffset =
          bufferOffsets.submeshesOffsets[materialHash][submeshIndex].uboOffset;

      // Descriptor set 2 - Submesh
      vkCmdBindDescriptorSets(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              m_basicMatPipelineVk.GetVkPipelineLayout(), 2,
                              submeshSets.size(), submeshSets.data(), 1,
                              &submeshUboOffset);

      BufferVkUtils::BindVBO(cmdBfr, vertexBuffers, vboOffsets, 1);
      BufferVkUtils::BindIBO<uint32_t>(cmdBfr, indexBuffer, 0);

      vkCmdDrawIndexed(cmdBfr, submesh->GetIndexBuffer()->GetCount(), 1, 0, 0,
                       0);

      ++submeshIndex;
    }
  }

  //
  // --- End renderpass --------------------------------------------------------
  m_renderPass.EndRenderPass(cmdBfr);

  ContextVk::CommandPool()->EndCommandBuffer(cmdBfr);
};

//
//
//
//
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
//
void GBufferRenderFlowVk::DestroyFramebuffers() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    if (m_framebuffers[i] != VK_NULL_HANDLE) {
      FramebufferVk::DestroyVkFramebuffer(ContextVk::Device()->GetDevice(),
                                          m_framebuffers[i]);
      m_framebuffers[i] = VK_NULL_HANDLE;
    }
  }
};

void GBufferRenderFlowVk::DestroyRenderPass() {
  if (m_renderPass.GetVkRenderPass() != VK_NULL_HANDLE) {
    m_renderPass.Destroy(ContextVk::Device()->GetDevice());
  }
};

void GBufferRenderFlowVk::DestroyAttachments() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    m_imagesVk.albedoImages[i].Destroy();
    m_imagesVk.depthImages[i].Destroy();
    m_imagesVk.normalImages[i].Destroy();
    m_imagesVk.roughnessMetallicImages[i].Destroy();
  }
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
