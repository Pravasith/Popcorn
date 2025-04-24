#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

//
// GRAPHICS CLASS ----------------------------------------------------

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GBufferDescriptorSetVk {
public:
  GBufferDescriptorSetVk() {
    PC_PRINT("CREATED", TagType::Constr, "GBufferDescriptorSetVk.h")
  };
  ~GBufferDescriptorSetVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "GBufferDescriptorSetVk.h")
  };

  void CreateDescriptorSetLayout() {};
  void CreateDescriptorSets() {};

private:
  enum class Bindings {
    CameraUBO,
    ModelMatrixUBO,
    BasicMaterialUBO,
    PbrMaterialUBO
  };

  std::unordered_map<Bindings,
                     std::pair<VkDescriptorSetLayout, VkDescriptorSet>>
      m_bindings;
  std::unordered_map<Bindings, std::vector<uint32_t>> m_bindingOffsets;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
