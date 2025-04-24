#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <vector>

//
// GRAPHICS CLASS ----------------------------------------------------

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class DescriptorPoolVk {
public:
  // --- UTILS -----------------------------------------------------------------
  static void CreateDescriptorPool(const VkDescriptorPoolCreateInfo &poolInfo,
                                   VkDescriptorPool *pool);
  static void DestroyDescriptorPool(VkDescriptorPool &pool);

  static void
  GetDefaultDescriptorPoolState(VkDescriptorPoolCreateInfo &poolInfo,
                                uint32_t maxDSets,
                                std::vector<VkDescriptorPoolSize> &poolSizes);

private:
  DescriptorPoolVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorPoolVk")
  };
  ~DescriptorPoolVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorPoolVk")
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
