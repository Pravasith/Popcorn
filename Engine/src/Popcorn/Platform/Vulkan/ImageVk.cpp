#include "ImageVk.h"
#include "ContextVk.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void ImageVk::GetDefaultImageCreateInfo(VkImageCreateInfo &imageInfo,
                                        uint32_t width, uint32_t height) {
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
};

void ImageVk::CreateVmaImage(const VkImageCreateInfo &imageCreateInfo,
                             const VmaAllocationCreateInfo &vmaCreateInfo) {
  VkResult result = vmaCreateImage(
      ContextVk::MemoryAllocator()->GetVMAAllocator(), &imageCreateInfo,
      &vmaCreateInfo, &m_image, &m_alloc, nullptr);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create VMA image.");
  }
};

void ImageVk::GetDefaultImageViewCreateInfo(VkImageViewCreateInfo &viewInfo,
                                            const VkImage &image) {
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;
};

void ImageVk::CreateImageView(const VkImageViewCreateInfo &imageViewInfo) {
  if (vkCreateImageView(ContextVk::Device()->GetDevice(), &imageViewInfo,
                        nullptr, &m_imageView) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create image view!");
  };
};

void ImageVk::Destroy() {
  const VkDevice device = ContextVk::Device()->GetDevice();

  if (m_imageView != VK_NULL_HANDLE) {
    vkDestroyImageView(device, m_imageView, nullptr);
    m_imageView = VK_NULL_HANDLE;
  }

  if (m_image != VK_NULL_HANDLE && m_alloc != nullptr) {
    vmaDestroyImage(ContextVk::MemoryAllocator()->GetVMAAllocator(), m_image,
                    m_alloc);
    m_image = VK_NULL_HANDLE;
    m_alloc = nullptr;
  }
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
