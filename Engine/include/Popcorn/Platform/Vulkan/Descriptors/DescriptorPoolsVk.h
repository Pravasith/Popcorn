#pragma once

#include "DescriptorLayoutsVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <array>
#include <cstdint>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum DescriptorPools {
  GlobalDescriptorsPool = 1,
  GBufferPool,
  LightingPool,
  CompositePool
};

class DPoolVk {
  friend class DescriptorPoolsVk;

public:
  template <DescriptorSets T, uint32_t Count>
  [[nodiscard]] std::array<VkDescriptorSet, Count> AllocateDescriptorSets(
      const VkDevice &device,
      const std::array<VkDescriptorSetLayout, Count> &layouts) {
    return DefaultAllocateDescriptorSets<Count>(device, layouts);
  };

  //
  // --- UTILS -------------------------------------------------------------
private:
  DPoolVk() { PC_PRINT("CREATED", TagType::Constr, "DPoolVk.h") }
  ~DPoolVk() { PC_PRINT("DESTROYED", TagType::Destr, "DPoolVk.h") }

  void Create(VkDescriptorPoolSize *poolSizes, uint32_t poolSizesCount,
              uint32_t maxSets);
  void CleanUp();

  template <uint32_t Count>
  [[nodiscard]] std::array<VkDescriptorSet, Count>
  DefaultAllocateDescriptorSets(
      const VkDevice &device,
      std::array<VkDescriptorSetLayout, Count> &layouts) {

#if PC_DEBUG
    m_setsAllocated += Count;
    if (m_setsAllocated >= m_maxSets) {
      PC_ERROR("Pool reached maximum set capacity of " << m_maxSets << ".",
               "DPoolVk");
    }
#endif

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pSetLayouts = layouts.data();
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = Count;
    allocInfo.pNext = nullptr;

    std::array<VkDescriptorSet, Count> sets{};

    if (vkAllocateDescriptorSets(device, &allocInfo, sets.data()) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to create descriptor set!");
    };

    return sets;
  };

private:
#if PC_DEBUG
  uint32_t m_setsAllocated = 0;
  uint32_t m_maxSets = 0;
#endif
  VkDescriptorPool m_pool = VK_NULL_HANDLE;
};

// TODO: Make it a growable pool
class DescriptorPoolsVk {

public:
  template <DescriptorPools T> [[nodiscard]] DPoolVk &GetPool(uint32_t count);
  void CleanUp();

public:
  [[nodiscard]] inline static DescriptorPoolsVk *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new DescriptorPoolsVk();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of DescriptorPoolsVk")
    };
  };

private:
  DescriptorPoolsVk() {
    PC_PRINT("CREATED", TagType::Constr, "DescriptorPoolsVk.h")
  }
  ~DescriptorPoolsVk(){
      PC_PRINT("DESTROYED", TagType::Destr, "DescriptorPoolsVk.h")}

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  DescriptorPoolsVk(const DescriptorPoolsVk &) = delete;
  DescriptorPoolsVk &operator=(const DescriptorPoolsVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  DescriptorPoolsVk(DescriptorPoolsVk &&) = delete;
  DescriptorPoolsVk &operator=(DescriptorPoolsVk &&) = delete;

private:
  static DescriptorPoolsVk *s_instance;
  std::unordered_map<DescriptorPools, DPoolVk> m_pools{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
