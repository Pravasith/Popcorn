#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <array>
#include <cstdint>
#include <vulkan/vulkan_core.h>

//
// DescriptorUtilsVk CLASS ----------------------------------------------------

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class DescriptorUtilsVk {
public:
  DescriptorUtilsVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorUtilsVk.h")
  };
  ~DescriptorUtilsVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorUtilsVk.h")
  };

  template <uint32_t Count>
  inline static void
  BindDescriptorSets(const VkCommandBuffer &cmdBfr,
                     const VkPipelineLayout &pipelineLayout,
                     const std::array<VkDescriptorSet, Count> &descriptorSets) {
    vkCmdBindDescriptorSets(cmdBfr, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, 0, Count, descriptorSets.data(), 0,
                            nullptr);
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
