#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
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
      PC_WARN("Trying to destroy a non-existant instance of MaterialHandler")
    };
  };

  [[nodiscard]] VkDescriptorSetLayout &
  GetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings);

  // Destroy cache after descriptor sets & pipelines are created & no longer
  // required
  void CleanUp();

  // --- UTILS -----------------------------------------------------------------
  [[nodiscard]] static VkDescriptorSetLayout CreateDescriptorSetLayout(
      const std::vector<VkDescriptorSetLayoutBinding> &bindings);

private:
  DescriptorSetLayoutsVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorSetLayoutsVk")
  };
  ~DescriptorSetLayoutsVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorSetLayoutsVk")
  };

  static size_t
  HashLayoutBindings(const std::vector<VkDescriptorSetLayoutBinding> &bindings);

private:
  static DescriptorSetLayoutsVk *s_instance;

  std::unordered_map<size_t, VkDescriptorSetLayout> m_layoutCache;
};

//
//
// -------------------------------------------------------------------------
// --- DESCRIPTOR POOL -----------------------------------------------------
//
class DescriptorPoolVk {
public:
  // --- UTILS -----------------------------------------------------------------
  static void CreateDescriptorPool(const VkDescriptorPoolCreateInfo &poolInfo,
                                   VkDescriptorPool &pool);
  static void DestroyDescriptorPool(VkDescriptorPool &pool);

  static void
  GetDefaultDescriptorPoolState(VkDescriptorPoolCreateInfo &poolInfo,
                                uint32_t maxDSets,
                                std::vector<VkDescriptorPoolSize> poolSizes);

private:
  DescriptorPoolVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorPoolVk")
  };
  ~DescriptorPoolVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorPoolVk")
  };
};

//
//
// -------------------------------------------------------------------------
// --- DESCRIPTOR SETS -----------------------------------------------------
//
class DescriptorSetsVk {
public:
  static void
  AllocateDescriptorSets(const VkDescriptorSetAllocateInfo &allocInfo,
                         std::vector<VkDescriptorSet> &descriptorSets);
  static void
  GetDefaultDescriptorSetAllocateState(const VkDescriptorSetLayout &layout,
                                       const VkDescriptorPool &pool,
                                       VkDescriptorSetAllocateInfo &allocInfo);

private:
  DescriptorSetsVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorSetsVk")
  };
  ~DescriptorSetsVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorSetsVk")
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
