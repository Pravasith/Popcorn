#pragma once

#include "GlobalMacros.h"
#include "Memory/MemoryDefsVk.h"
#include "Popcorn/Core/Base.h"
#include "Uniforms.h"
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum DescriptorSetGroups { GBufferGroup = 1, LightingGroup, CompositeGroup };

enum DescriptorSets {
  CameraSet = 1, // 1 Static UBO - camera matrix - one camera at a time
                 //
  BasicMatSet,   // 1 Dynamic UBO - basic material values
  PbrMatSet,     // 1 Dynamic UBO - pbr material values
                 //
  SubmeshSet,    // 1 Dynamic UBO - model matrix
                 //
  LightingSet,   // 1 Static SSBO - lights info (pos, rot, scale)
                 // 1 Sampler - albedo image & sampler
                 // 1 Sampler - depth image & sampler
                 // 1 Sampler - normal image & sampler
                 //
  PresentSet     // 1 Sampler - light image & sampler
                 // 1 Sampler - depth image & sampler (for depth fx)
                 // 1 Sampler - normal image & sampler (for depth fx)
};

class DescriptorLayoutsVk {
public:
  template <DescriptorSets T> [[nodiscard]] VkDescriptorSetLayout &GetLayout();

  template <DescriptorSets T>
  [[nodiscard]] static VkDeviceSize
  GetDescriptorBufferRange(const VkPhysicalDeviceLimits &limits) {
    if constexpr (T == DescriptorSets::CameraSet) {
      return PC_AlignCeil(UniformDefs::CameraUniform::size,
                          limits.minUniformBufferOffsetAlignment);
    } else if constexpr (T == DescriptorSets::BasicMatSet) {
      return PC_AlignCeil(UniformDefs::BasicMaterialUniform::size,
                          limits.minUniformBufferOffsetAlignment);
    } else if constexpr (T == DescriptorSets::PbrMatSet) {
      return PC_AlignCeil(UniformDefs::PbrMaterialUniform::size,
                          limits.minUniformBufferOffsetAlignment);
    } else if constexpr (T == DescriptorSets::SubmeshSet) {
      return PC_AlignCeil(UniformDefs::SubmeshUniform::size,
                          limits.minUniformBufferOffsetAlignment);
    } else if constexpr (T == DescriptorSets::PresentSet) {
    } else if constexpr (T == DescriptorSets::LightingSet) {
      return PC_AlignCeil(UniformDefs::LightUniform::size,
                          limits.minStorageBufferOffsetAlignment);
    };
  };

public:
  [[nodiscard]] static VkDescriptorSetLayout CreateDescriptorSetLayout(
      const std::vector<VkDescriptorSetLayoutBinding> &bindings);
  static size_t
  HashLayoutBindings(const std::vector<VkDescriptorSetLayoutBinding> &bindings);
  void CleanUp();

public:
  [[nodiscard]] inline static DescriptorLayoutsVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new DescriptorLayoutsVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN(
          "Trying to destroy a non-existant instance of DescriptorLayoutsVk")
    };
  };

private:
  [[nodiscard]] VkDescriptorSetLayout &
  GetCachedLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings);

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  DescriptorLayoutsVk(const DescriptorLayoutsVk &) = delete;
  DescriptorLayoutsVk &operator=(const DescriptorLayoutsVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  DescriptorLayoutsVk(DescriptorLayoutsVk &&) = delete;
  DescriptorLayoutsVk &operator=(DescriptorLayoutsVk &&) = delete;

  DescriptorLayoutsVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorLayoutsVk.h")
  };
  ~DescriptorLayoutsVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorLayoutsVk.h")
  };

private:
  static DescriptorLayoutsVk *s_instance;

  std::unordered_map<size_t, VkDescriptorSetLayout> m_layoutCache;
  std::unordered_map<DescriptorSets, VkDescriptorSetLayout>
      m_layouts; // VkDescriptorSetLayout is just a uint64_t so cheap to copy
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
