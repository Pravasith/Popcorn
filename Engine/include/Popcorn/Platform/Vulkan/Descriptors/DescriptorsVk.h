#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// -------------------------------------------------------------------------
// --- DESCRIPTOR SET LAYOUTS ----------------------------------------------
//
class DescriptorSetLayoutsVk {
public:
  [[nodiscard]] inline static DescriptorSetLayoutsVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new DescriptorSetLayoutsVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN(
          "Trying to destroy a non-existant instance of DescriptorSetLayoutsVk")
    };
  };

  [[nodiscard]] VkDescriptorSetLayout &
  GetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings);

  // Destroy cache after descriptor sets & pipelines are created & no longer
  // required
  void CleanUp();

  // --- UTILS -----------------------------------------------------------------
  static void GetDefaultDescriptorSetLayoutBindingState(
      VkDescriptorSetLayoutBinding &binding);

private:
  DescriptorSetLayoutsVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorSetLayoutsVk")
  };
  ~DescriptorSetLayoutsVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorSetLayoutsVk")
  };

  // --- UTILS -----------------------------------------------------------------
  [[nodiscard]] static VkDescriptorSetLayout CreateDescriptorSetLayout(
      const std::vector<VkDescriptorSetLayoutBinding> &bindings);

  static size_t
  HashLayoutBindings(const std::vector<VkDescriptorSetLayoutBinding> &bindings);

private:
  static DescriptorSetLayoutsVk *s_instance;

  std::unordered_map<size_t, VkDescriptorSetLayout> m_layoutCache;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
