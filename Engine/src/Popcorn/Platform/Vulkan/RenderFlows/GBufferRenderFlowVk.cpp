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

  //
  //
  // --- Create G-Buffer Images ------------------------------------------------
  VkImageCreateInfo albedoMapInfo;
  ImageVk::GetDefaultImageCreateInfo(albedoMapInfo, width, height);
  albedoMapInfo.format = VK_FORMAT_R8G8B8A8_UNORM;

  VkImageCreateInfo depthMapInfo;
  ImageVk::GetDefaultImageCreateInfo(depthMapInfo, width, height);
  depthMapInfo.format = VK_FORMAT_D32_SFLOAT;

  VkImageCreateInfo normalMapInfo;
  ImageVk::GetDefaultImageCreateInfo(normalMapInfo, width, height);
  normalMapInfo.format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;

  VmaAllocationCreateInfo albedoAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo depthAlloc{.usage = VMA_MEMORY_USAGE_AUTO};
  VmaAllocationCreateInfo normalAlloc{.usage = VMA_MEMORY_USAGE_AUTO};

  ImageVk albedoImage{};
  ImageVk depthImage{};
  ImageVk normalImage{};

  albedoImage.CreateVmaImage(albedoMapInfo, albedoAlloc);
  depthImage.CreateVmaImage(depthMapInfo, depthAlloc);
  normalImage.CreateVmaImage(normalMapInfo, normalAlloc);

  //
  //
  // --- Create G-Buffer Image Views -------------------------------------------
  VkImageViewCreateInfo albedoViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(albedoViewInfo,
                                         albedoImage.GetVkImage());
  albedoViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;

  VkImageViewCreateInfo depthViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(depthViewInfo,
                                         depthImage.GetVkImage());
  depthViewInfo.format = VK_FORMAT_D32_SFLOAT;

  VkImageViewCreateInfo normalViewInfo{};
  ImageVk::GetDefaultImageViewCreateInfo(normalViewInfo,
                                         normalImage.GetVkImage());
  normalViewInfo.format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;

  albedoImage.CreateImageView(albedoViewInfo);
  depthImage.CreateImageView(depthViewInfo);
  normalImage.CreateImageView(normalViewInfo);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
