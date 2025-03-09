#pragma once

#include "DeviceVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

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
      PC_WARN("Trying to destroy a non-existant instance of MaterialHandler")
    };
  };

  // Destroy cache after descriptor sets & pipelines are created & no longer
  // required
  void CleanUp() {
    auto &device = m_deviceVk->GetDevice();

    for (auto &[_, layout] : m_layoutCache) {
      vkDestroyDescriptorSetLayout(device, layout, nullptr);
    }

    m_layoutCache.clear();
  };

  VkDescriptorSetLayout &
  GetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings);

  //
  // --- UTILS -----------------------------------------------------------------
  [[nodiscard]] static VkDescriptorSetLayout CreateDescriptorSetLayout(
      const std::vector<VkDescriptorSetLayoutBinding> &bindings);

private:
  DescriptorSetLayoutsVk() {
    m_deviceVk = DeviceVk::Get();
    PC_PRINT("CREATED", TagType::Constr, "DescriptorSetLayoutsVk")
  };
  ~DescriptorSetLayoutsVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorSetLayoutsVk")
  };

  static size_t
  HashLayoutBindings(const std::vector<VkDescriptorSetLayoutBinding> &bindings);

private:
  static DescriptorSetLayoutsVk *s_instance;
  DeviceVk *m_deviceVk;

  std::unordered_map<size_t, VkDescriptorSetLayout> m_layoutCache;
};

class DescriptorPoolVk {
public:
  DescriptorPoolVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorPoolVk")
  };
  ~DescriptorPoolVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorPoolVk")
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
