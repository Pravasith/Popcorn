#include "RenderFlows/GBufferRenderFlowVk.h"
#include "AttachmentVk.h"
#include "ContextVk.h"
#include "DescriptorLayoutsVk.h"
#include "DescriptorPoolsVk.h"
#include "FramebuffersVk.h"
#include "ImageVk.h"
#include "RenderPassVk.h"
#include <algorithm>
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
  albedoAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  AttachmentVk::GetDefaultAttachmentDescription(depthAttachment);
  depthAttachment.format = depthFormat;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

  VkAttachmentDescription normalAttachment{};
  AttachmentVk::GetDefaultAttachmentDescription(normalAttachment);
  normalAttachment.format = normalFormat;
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
  constexpr uint32_t maxFIF = MAX_FRAMES_IN_FLIGHT;

  VkDescriptorSetLayout &cameraLayout =
      layouts->GetLayout<DescriptorSets::CameraSet>();
  std::array<VkDescriptorSetLayout, maxFIF> cameraLayouts{};
  std::fill(cameraLayouts.begin(), cameraLayouts.end(), cameraLayout);

  VkDescriptorSetLayout &gameObjectLayout =
      layouts->GetLayout<DescriptorSets::GameObjectSet>();
  std::array<VkDescriptorSetLayout, maxFIF> gameObjectLayouts{};
  std::fill(gameObjectLayouts.begin(), gameObjectLayouts.end(),
            gameObjectLayout);

  VkDescriptorSetLayout &basicMatLayout =
      layouts->GetLayout<DescriptorSets::BasicMatSet>();
  std::array<VkDescriptorSetLayout, maxFIF> basicMatLayouts{};
  std::fill(basicMatLayouts.begin(), basicMatLayouts.end(), basicMatLayout);

  VkDescriptorSetLayout &pbrMatLayout =
      layouts->GetLayout<DescriptorSets::PbrMatSet>();
  std::array<VkDescriptorSetLayout, maxFIF> pbrMatLayouts{};
  std::fill(pbrMatLayouts.begin(), pbrMatLayouts.end(), pbrMatLayout);

  DPoolVk &gBufferPool =
      pools->GetPool<DescriptorPools::GBufferPool>(); // Creates pool if it
                                                      // doesn't exist

  const VkDevice &device = ContextVk::Device()->GetDevice();

  std::vector<VkDescriptorSet> cameraSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::CameraSet, maxFIF>(
          device, cameraLayouts);
  std::vector<VkDescriptorSet> gameObjectSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::GameObjectSet, maxFIF>(
          device, gameObjectLayouts);
  std::vector<VkDescriptorSet> basicMatSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::BasicMatSet, maxFIF>(
          device, basicMatLayouts);
  std::vector<VkDescriptorSet> pbrMatSets =
      gBufferPool.AllocateDescriptorSets<DescriptorSets::PbrMatSet, maxFIF>(
          device, pbrMatLayouts);
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
