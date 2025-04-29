#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <unordered_map>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class DescriptorSetGroups {
  GBufferGroup = 1,
  LightingGroup,
  CompositeGroup
};

enum class DescriptorSets {
  CameraSet = 1, // 1 Static UBO - Camera matrix
  GameObjectSet, // 1 Dynamic UBO - Model matrix
  BasicMatSet,   // 1 Dynamic UBO - Basic material matrix
  PbrMatSet,     // 1 Dynamic UBO - Pbr material matrix

  LightingSet, // 1 Dynamic UBO - Lights info (pos, rot, scale)
               // 1 Sampler - Albedo image & sampler
               // 1 Sampler - Depth image & sampler
               // 1 Sampler - Normal image & sampler

  CompositeSet
};

class DescriptorLayoutsVk {
public:
  template <DescriptorSets T> [[nodiscard]] VkDescriptorSetLayout &GetLayout();
  template <DescriptorSets T> [[nodiscard]] VkDescriptorSet &GetDescriptorSet();

  void CleanUp() { m_layouts.clear(); };

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
  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  DescriptorLayoutsVk(const DescriptorLayoutsVk &) = delete;
  DescriptorLayoutsVk &operator=(const DescriptorLayoutsVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  DescriptorLayoutsVk(DescriptorLayoutsVk &&) = delete;
  DescriptorLayoutsVk &operator=(DescriptorLayoutsVk &&) = delete;

private:
  DescriptorLayoutsVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorLayoutsVk.h")
  };
  ~DescriptorLayoutsVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorLayoutsVk.h")
  };

private:
  static DescriptorLayoutsVk *s_instance;
  std::unordered_map<DescriptorSets, VkDescriptorSetLayout>
      m_layouts; // VkDescriptorSetLayout is just a uint64_t so cheap to copy
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
