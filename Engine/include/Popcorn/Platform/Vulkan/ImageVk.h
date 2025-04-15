#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

//
// GRAPHICS CLASS ----------------------------------------------------

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

  [[nodiscard]] const VkAttachmentDescription &
  GetAttachmentDescription() const {
    return m_imageDescription;
  };

  void SetAttachmentDescription(const VkAttachmentDescription &description) {
    m_imageDescription = description;
  };

  static void GetDefaultImageCreateInfo(VkImageCreateInfo &imageInfo,
                                        uint32_t width, uint32_t height);
  static void GetDefaultImageViewCreateInfo(VkImageViewCreateInfo &viewInfo,
                                            const VkImage &image);

private:
  VkImage m_image = VK_NULL_HANDLE;
  VkImageView m_imageView = VK_NULL_HANDLE;
  VkAttachmentDescription m_imageDescription{};

  VmaAllocation m_alloc = nullptr;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
