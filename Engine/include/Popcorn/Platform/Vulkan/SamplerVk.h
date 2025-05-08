#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class SamplerVk {
public:
  SamplerVk() { PC_PRINT("CREATED", TagType::Constr, "SamplerVk.h") };
  ~SamplerVk() { PC_PRINT("DESTROYED", TagType::Destr, "SamplerVk.h") };

  // void CreateSampler

private:
  VkSampler m_sampler = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
