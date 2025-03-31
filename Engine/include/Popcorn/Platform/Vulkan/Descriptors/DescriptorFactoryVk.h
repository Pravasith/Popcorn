#pragma once

#include "GlobalMacros.h"
#include "PipelineFactoryVk.h"
#include "Popcorn/Core/Base.h"
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class DescriptorFactoryVk {
public:
  // Create DescriptorSet Layout
  // Create Vk memory and set offsets
  // Create VMA crap
  // Create DescriptorSets

  template <Pipelines T> void CreateDescriptorSetLayouts();
  template <Pipelines T>
  [[nodiscard]] const std::vector<VkDescriptorSetLayout> &
  GetDescriptorSetLayouts() const {
    return m_descriptorLayoutMap.at(T);
  };

public:
  [[nodiscard]] inline static DescriptorFactoryVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new DescriptorFactoryVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN(
          "Trying to destroy a non-existant instance of DescriptorFactoryVk")
    };
  };

private:
  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  DescriptorFactoryVk(const DescriptorFactoryVk &) = delete;
  DescriptorFactoryVk &operator=(const DescriptorFactoryVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  DescriptorFactoryVk(DescriptorFactoryVk &&) = delete;
  DescriptorFactoryVk &operator=(DescriptorFactoryVk &&) = delete;

private:
  DescriptorFactoryVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorFactoryVk.h")
  };
  ~DescriptorFactoryVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorFactoryVk.h")
  };

private:
  static DescriptorFactoryVk *s_instance;
  std::unordered_map<Pipelines, std::vector<VkDescriptorSetLayout>>
      m_descriptorLayoutMap;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
