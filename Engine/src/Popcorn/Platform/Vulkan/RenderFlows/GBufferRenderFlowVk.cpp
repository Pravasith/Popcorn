#include "RenderFlows/GBufferRenderFlowVk.h"
#include "ContextVk.h"
#include "ImageVk.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void GBufferRenderFlowVk::CreateAttachments() {
  const auto &swapchainExtent = ContextVk::Swapchain()->GetSwapchainExtent();

  const auto &width = swapchainExtent.width;
  const auto &height = swapchainExtent.height;

  VkImageCreateInfo albedoMapInfo;
  VkImageCreateInfo depthMapInfo;
  VkImageCreateInfo normalMapInfo;

  ImageVk::GetDefaultImageCreateInfo(albedoMapInfo, width, height);
  ImageVk::GetDefaultImageCreateInfo(depthMapInfo, width, height);
  ImageVk::GetDefaultImageCreateInfo(normalMapInfo, width, height);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
