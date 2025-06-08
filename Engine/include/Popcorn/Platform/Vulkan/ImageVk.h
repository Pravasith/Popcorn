#pragma once

#include "GlobalMacros.h"
#include "MemoryAllocatorVk.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class ImageVk {
public:
  ImageVk() { PC_PRINT("CREATED", TagType::Constr, "ImageVk.h") };
  ~ImageVk() { PC_PRINT("DESTROYED", TagType::Destr, "ImageVk.h") };

  void Destroy();

  void CreateVmaImage(const VkImageCreateInfo &imageCreateInfo,
                      const VmaAllocationCreateInfo &vmaCreateInfo);
  void CreateImageView(const VkImageViewCreateInfo &imageViewInfo);

  [[nodiscard]] const VkImage &GetVkImage() const { return m_image; };
  [[nodiscard]] const VkImageView &GetVkImageView() const {
    return m_imageView;
  };

  static void GetDefaultImageCreateInfo(VkImageCreateInfo &imageInfo,
                                        uint32_t width, uint32_t height,
                                        VkFormat format);
  static void GetDefaultImageViewCreateInfo(VkImageViewCreateInfo &viewInfo,
                                            const VkImage &image,
                                            VkFormat format);

  void SetSwapchainImageData(VkImage &swapchainImage,
                             VkImageView &swapchainImageView, VkFormat format) {
    m_image = swapchainImage;
    m_imageView = swapchainImageView;
    m_format = format;
  };

public:
  // --- UTILS -------------------------------------------------------------
  static VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates,
                                      VkImageTiling tiling,
                                      VkFormatFeatureFlags features);

  const bool FormatHasStencilComponent() const {
    PC_ASSERT(m_format != VK_FORMAT_UNDEFINED, "m_format is null");
    return m_format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           m_format == VK_FORMAT_D24_UNORM_S8_UINT;
  }

private:
  VkImage m_image = VK_NULL_HANDLE;
  VkImageView m_imageView = VK_NULL_HANDLE;
  VmaAllocation m_alloc = nullptr;
  VkFormat m_format = VK_FORMAT_UNDEFINED;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
