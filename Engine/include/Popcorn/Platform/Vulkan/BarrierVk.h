#pragma once

#include "GlobalMacros.h"
#include "ImageVk.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BarrierUtilsVk {
public:
  BarrierUtilsVk() { PC_PRINT("CREATED", TagType::Constr, "BarrierUtilsVk.h") };
  ~BarrierUtilsVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "BarrierUtilsVk.h")
  };

  static void GetDefaultImageBarrierInfo(const VkImage &image,
                                         const VkImageLayout &oldLayout,
                                         const VkImageLayout &newLayout,
                                         const VkImageAspectFlags &aspect,
                                         VkImageMemoryBarrier &barrier) {
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspect;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
  };
};

enum LayoutTransitions {
  ColorAttachmentToShaderRead = 1,
  DepthAttachmentToShaderRead,
  ColorAttachmentToPresentSrc
};

enum LayoutTransitionDirection { Forward, Backward };

struct PipelineStagesSyncInfo {
  VkPipelineStageFlags srcStageMask = 0;
  VkPipelineStageFlags dstStageMask = 0;
};

template <LayoutTransitions T> class ImageBarrierVk {
public:
  ImageBarrierVk() { PC_PRINT("CREATED", TagType::Constr, "ImageBarrierVk.h") }
  ~ImageBarrierVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "ImageBarrierVk.h")
  }

  void Init(const ImageVk *imageVk) {
    m_imageVk = imageVk;
    Create();
  }

  void RecordBarrierCommand(const VkCommandBuffer &commandBuffer) {
    vkCmdPipelineBarrier(commandBuffer, m_syncStages.srcStageMask,
                         m_syncStages.dstStageMask, 0, 0, nullptr, 0, nullptr,
                         1, &m_imageBarrier);
  }

  [[nodiscard]] const LayoutTransitionDirection &GetDirection() {
    return m_direction;
  }

  void ReverseDirection() {
    m_direction = m_direction == Forward ? Backward : Forward;
    Create();
  }

private:
  void Create();

private:
  VkImageMemoryBarrier m_imageBarrier{};
  const ImageVk *m_imageVk = nullptr;

  PipelineStagesSyncInfo m_syncStages;

  LayoutTransitionDirection m_direction = LayoutTransitionDirection::Forward;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
