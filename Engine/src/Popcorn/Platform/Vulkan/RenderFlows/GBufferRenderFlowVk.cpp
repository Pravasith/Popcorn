#include "RenderFlows/GBufferRenderFlowVk.h"
#include "AttachmentVk.h"
#include "CommonVk.h"
#include "ContextVk.h"
#include "DescriptorLayoutsVk.h"
#include "DescriptorPoolsVk.h"
#include "FramebuffersVk.h"
#include "ImageVk.h"
#include "Memory/MemoryVk.h"
#include "RenderPassVk.h"
#include <algorithm>
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

  const auto albedoFormat = VK_FORMAT_R8G8B8A8_UNORM;
  const auto depthFormat = VK_FORMAT_D32_SFLOAT;
  const auto normalFormat = VK_FORMAT_A2B10G10R10_UNORM_PACK32;

  //
  //
  // --- Create g-buffer images ------------------------------------------------
  VkImageCreateInfo albedoImageInfo;
  ImageVk::GetDefaultImageCreateInfo(albedoImageInfo, width, height);
  albedoImageInfo.format = albedoFormat;
  albedoImageInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VkImageCreateInfo depthImageInfo;
  ImageVk::GetDefaultImageCreateInfo(depthImageInfo, width, height);
  depthImageInfo.format = depthFormat;
  depthImageInfo.usage =
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VkImageCreateInfo normalImageInfo;
  ImageVk::GetDefaultImageCreateInfo(normalImageInfo, width, height);
  normalImageInfo.format = normalFormat;
  normalImageInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

  VmaAllocationCreateInfo albedoAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo depthAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo normalAlloc{.usage = VMA_MEMORY_USAGE_AUTO};

  ImageVk &albedoImage = m_imagesVk.albedoImage;
  ImageVk &depthImage = m_imagesVk.depthImage;
  ImageVk &normalImage = m_imagesVk.normalImage;

  albedoImage.CreateVmaImage(albedoImageInfo, albedoAlloc);
  depthImage.CreateVmaImage(depthImageInfo, depthAlloc);
  normalImage.CreateVmaImage(normalImageInfo, normalAlloc);

  //
  //
  // --- Create g-buffer image-views -------------------------------------------
  VkImageViewCreateInfo albedoViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(albedoViewInfo,
                                         albedoImage.GetVkImage());
  albedoViewInfo.format = albedoFormat;
  albedoViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  VkImageViewCreateInfo depthViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(depthViewInfo,
                                         depthImage.GetVkImage());
  depthViewInfo.format = depthFormat;
  depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

  VkImageViewCreateInfo normalViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(normalViewInfo,
                                         normalImage.GetVkImage());
  normalViewInfo.format = normalFormat;
  normalViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  albedoImage.CreateImageView(albedoViewInfo);
  depthImage.CreateImageView(depthViewInfo);
  normalImage.CreateImageView(normalViewInfo);

  //
  //
  // --- Create g-buffer attachments -------------------------------------------
  VkAttachmentDescription albedoAttachment{};
  AttachmentVk::GetDefaultAttachmentDescription(albedoAttachment);
  albedoAttachment.format = albedoFormat;
  albedoAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  albedoAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  AttachmentVk::GetDefaultAttachmentDescription(depthAttachment);
  depthAttachment.format = depthFormat;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

  VkAttachmentDescription normalAttachment{};
  AttachmentVk::GetDefaultAttachmentDescription(normalAttachment);
  normalAttachment.format = normalFormat;
  normalAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  normalAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  m_attachmentsVk.albedoAttachment.SetImageVk(&albedoImage);
  m_attachmentsVk.depthAttachment.SetImageVk(&depthImage);
  m_attachmentsVk.normalAttachment.SetImageVk(&normalImage);

  m_attachmentsVk.albedoAttachment.SetAttachmentDescription(albedoAttachment);
  m_attachmentsVk.depthAttachment.SetAttachmentDescription(depthAttachment);
  m_attachmentsVk.normalAttachment.SetAttachmentDescription(normalAttachment);
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
  //
  // --- Attachments -----------------------------------------------------------
  VkAttachmentDescription attachments[]{
      m_attachmentsVk.albedoAttachment.GetAttachmentDescription(),
      m_attachmentsVk.depthAttachment.GetAttachmentDescription(),
      m_attachmentsVk.normalAttachment.GetAttachmentDescription()};

  //
  // --- Attachment references -------------------------------------------------
  VkAttachmentReference albedoRef{};
  AttachmentVk::GetAttachmentRef(albedoRef, 0);
  albedoRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthRef{};
  AttachmentVk::GetAttachmentRef(depthRef, 1);
  depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

  VkAttachmentReference normalRef{};
  AttachmentVk::GetAttachmentRef(normalRef, 2);
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
  // dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  //
  // --- Renderpass ------------------------------------------------------------
  VkRenderPassCreateInfo renderPassInfo{};
  RenderPassVk::GetDefaultRenderPassCreateInfo(renderPassInfo);
  renderPassInfo.attachmentCount = 3;
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
void GBufferRenderFlowVk::CreateFramebuffer() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  std::vector<VkImageView> attachments = {
      m_imagesVk.albedoImage.GetVkImageView(),
      m_imagesVk.depthImage.GetVkImageView(),
      m_imagesVk.normalImage.GetVkImageView(),
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

//
//
//
//
//
// --- CREATE DESCRIPTORS ------------------------------------------------------
// --- CREATE DESCRIPTORS ------------------------------------------------------
// --- CREATE DESCRIPTORS ------------------------------------------------------
//
void GBufferRenderFlowVk::CreateAndAllocDescriptors() {
  auto *layouts = ContextVk::DescriptorLayouts();
  auto *pools = ContextVk::DescriptorPools();
  auto *device = ContextVk::Device();
  auto *memory = ContextVk::Memory();

  VkPhysicalDeviceProperties properties{};
  device->GetPhysicalDeviceProperties(properties);

  constexpr uint32_t maxFIF = MAX_FRAMES_IN_FLIGHT;

  DPoolVk &gBufferPool = pools->GetPool<DescriptorPools::GBufferPool>(
      MAX_FRAMES_IN_FLIGHT); // Creates pool if it
                             // doesn't exist

  VkDescriptorSetLayout &cameraLayout =
      layouts->GetLayout<DescriptorSets::CameraSet>();
  std::array<VkDescriptorSetLayout, maxFIF> cameraLayouts{};
  std::fill(cameraLayouts.begin(), cameraLayouts.end(), cameraLayout);

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
  // - Camera - Static UBO
  // - Submesh - Dynamic UBO
  // - BasicMat - Dynamic UBO
  // - PbrMat - Dynamic UBO
  //
  //
  // Descriptor set will be cleaned automatically when pools are destroyed
  m_descriptorSetsVk.cameraSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::CameraSet, maxFIF>(
          device->GetDevice(), cameraLayouts);
  m_descriptorSetsVk.submeshSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::SubmeshSet, maxFIF>(
          device->GetDevice(), submeshLayouts);
  m_descriptorSetsVk.basicMatSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::BasicMatSet, maxFIF>(
          device->GetDevice(), basicMatLayouts);
  m_descriptorSetsVk.pbrMatSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::PbrMatSet, maxFIF>(
          device->GetDevice(), pbrMatLayouts);

  // Bind sets with buffers
  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
    VkDescriptorBufferInfo cameraBufferInfo{};
    cameraBufferInfo.buffer = memory->GetUboSet(i);
    cameraBufferInfo.offset = memory->GetBufferViews().camerasUbo.offset;
    cameraBufferInfo.range = DescriptorLayoutsVk::GetDescriptorBufferRange<
        DescriptorSets::CameraSet>(properties.limits);

    VkDescriptorBufferInfo submeshBufferInfo{};
    submeshBufferInfo.buffer = memory->GetUboSet(i);
    submeshBufferInfo.offset = memory->GetBufferViews().submeshUbo.offset;
    submeshBufferInfo.range = DescriptorLayoutsVk::GetDescriptorBufferRange<
        DescriptorSets::SubmeshSet>(properties.limits);

    VkDescriptorBufferInfo basicMatBufferInfo{};
    basicMatBufferInfo.buffer = memory->GetUboSet(i);
    basicMatBufferInfo.offset = memory->GetBufferViews().basicMatUbo.offset;
    basicMatBufferInfo.range = DescriptorLayoutsVk::GetDescriptorBufferRange<
        DescriptorSets::BasicMatSet>(properties.limits);

    VkDescriptorBufferInfo pbrMatBufferInfo{};
    pbrMatBufferInfo.buffer = memory->GetUboSet(i);
    pbrMatBufferInfo.offset = memory->GetBufferViews().pbrMatUbo.offset;
    pbrMatBufferInfo.range = DescriptorLayoutsVk::GetDescriptorBufferRange<
        DescriptorSets::PbrMatSet>(properties.limits);

    VkWriteDescriptorSet cameraWrite{};
    cameraWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    cameraWrite.dstSet = m_descriptorSetsVk.cameraSets[i];
    cameraWrite.dstBinding = 0;
    cameraWrite.dstArrayElement = 0;
    cameraWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraWrite.descriptorCount = 1;
    cameraWrite.pBufferInfo = &cameraBufferInfo;
    cameraWrite.pImageInfo = nullptr;
    cameraWrite.pTexelBufferView = nullptr;

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

    std::vector<VkWriteDescriptorSet> writes{cameraWrite, submeshWrite,
                                             basicMatWrite, pbrMatWrite};

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
  // - Vertexbuffer layout
  // - Enabled shader stage bits
  // - Material-type specific shader codes (for custom shaders, user needs to
  //   provide as attachment in the material)
  // - Custom shader data - uniforms & defines
};

//
//
//
//
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
// --- CLEAN UP ----------------------------------------------------------------
//
void GBufferRenderFlowVk::DestroyFramebuffer() {
  if (m_framebuffer != VK_NULL_HANDLE) {
    FramebuffersVk::DestroyVkFramebuffer(ContextVk::Device()->GetDevice(),
                                         m_framebuffer);
    m_framebuffer = VK_NULL_HANDLE;
  }
};

void GBufferRenderFlowVk::DestroyRenderPass() {
  if (m_renderPass.GetVkRenderPass() != VK_NULL_HANDLE) {
    m_renderPass.Destroy(ContextVk::Device()->GetDevice());
  }
};

void GBufferRenderFlowVk::DestroyAttachments() {
  m_imagesVk.albedoImage.Destroy();
  m_imagesVk.depthImage.Destroy();
  m_imagesVk.normalImage.Destroy();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
