#pragma once

#include "ContextVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class SamplerVk {
public:
  SamplerVk() { PC_PRINT("CREATED", TagType::Constr, "SamplerVk.h") };
  ~SamplerVk() { PC_PRINT("DESTROYED", TagType::Destr, "SamplerVk.h") };

  static void
  GetDefaultSamplerCreateInfoValues(VkSamplerCreateInfo &createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    auto *device = ContextVk::Device();
    VkPhysicalDeviceProperties properties{};
    device->GetPhysicalDeviceProperties(properties);

    createInfo.anisotropyEnable = VK_TRUE;
    createInfo.maxAnisotropy =
        std::min(properties.limits.maxSamplerAnisotropy, 8.0f);
    createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.mipLodBias = 0.0f;
    createInfo.minLod = 0.0f;
    createInfo.maxLod = 0.0f;
  };

  void Create(const VkSamplerCreateInfo &createInfo) {
    // error in the funtion
    if (vkCreateSampler(ContextVk::Device()->GetDevice(), &createInfo, nullptr,
                        &m_sampler) != VK_SUCCESS) {
      throw std::runtime_error("failed to create sampler!");
    }
  }

  void Destroy() {
    PC_ASSERT(m_sampler != VK_NULL_HANDLE,
              "m_sampler is VK_NULL_HANDLE, attempt to destroy failed");
    vkDestroySampler(ContextVk::Device()->GetDevice(), m_sampler, nullptr);
  };

private:
  VkSampler m_sampler = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
