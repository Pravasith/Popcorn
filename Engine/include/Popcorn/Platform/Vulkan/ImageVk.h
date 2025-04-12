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

  void CreateVkImage(const VkImageCreateInfo &imageCreateInfo);
  static void GetDefaultImageCreateInfo(VkImageCreateInfo &imageInfo,
                                        uint32_t width, uint32_t height);

private:
  VkImage m_image = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
