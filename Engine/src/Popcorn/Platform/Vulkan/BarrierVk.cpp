#include "BarrierVk.h"
#include "Popcorn/Core/Assert.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <>
void ImageBarrierVk<LayoutTransitions::ColorAttachmentToShaderRead>::Create() {
  PC_ASSERT(m_imageVk, "ImageVk is null");

  if (m_direction == LayoutTransitionDirection::Forward) {
    BarrierUtilsVk::GetDefaultImageBarrierInfo(
        m_imageVk->GetVkImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT,
        m_imageBarrier);
    m_imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    m_imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    m_syncStages.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    m_syncStages.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    BarrierUtilsVk::GetDefaultImageBarrierInfo(
        m_imageVk->GetVkImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT,
        m_imageBarrier);

    m_imageBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    m_imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    m_syncStages.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    m_syncStages.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  }
}

template <>
void ImageBarrierVk<LayoutTransitions::DepthAttachmentToShaderRead>::Create() {
  PC_ASSERT(m_imageVk, "ImageVk is null");

  bool hasStencil = m_imageVk->FormatHasStencilComponent();
  VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  if (hasStencil)
    aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

  VkImageLayout initialLayout, finalLayout;
  VkAccessFlags srcAccessMask, dstAccessMask;

  if (m_direction == LayoutTransitionDirection::Forward) {
    initialLayout = hasStencil
                        ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                        : VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    finalLayout = hasStencil ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                             : VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;

    srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    m_syncStages.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    m_syncStages.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    initialLayout = hasStencil ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                               : VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
    finalLayout = hasStencil ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                             : VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

    srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    m_syncStages.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    m_syncStages.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  }

  BarrierUtilsVk::GetDefaultImageBarrierInfo(m_imageVk->GetVkImage(),
                                             initialLayout, finalLayout,
                                             aspectMask, m_imageBarrier);
  m_imageBarrier.srcAccessMask = srcAccessMask;
  m_imageBarrier.dstAccessMask = dstAccessMask;
};

template <>
void ImageBarrierVk<LayoutTransitions::ColorAttachmentToPresentSrc>::Create() {
  PC_ASSERT(m_imageVk, "ImageVk is null");

  BarrierUtilsVk::GetDefaultImageBarrierInfo(
      m_imageVk->GetVkImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT,
      m_imageBarrier);

  m_imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  m_imageBarrier.dstAccessMask = 0;

  m_syncStages.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  m_syncStages.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
