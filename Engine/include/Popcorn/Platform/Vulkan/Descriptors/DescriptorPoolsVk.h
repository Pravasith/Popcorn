#pragma once

#include "DescriptorFactoryVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class DescriptorPools { GBufferPool = 1, LightingPool, CompositePool };

class DPoolVk {
  friend class DescriptorPoolsVk;

public:
  template <DescriptorSets T>
  std::vector<VkDescriptorSet>
  AllocateDescriptorSets(const VkDescriptorSetLayout *layouts);

  //
  // --- UTILS -------------------------------------------------------------
  template <uint32_t Count>
  void DefaultAllocateDescriptorSets(const VkDevice &device,
                                     const VkDescriptorSetLayout *layouts,
                                     std::vector<VkDescriptorSet> &sets) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pSetLayouts = layouts;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = Count;
    allocInfo.pNext = nullptr;

    sets.resize(Count);

    if (vkAllocateDescriptorSets(device, &allocInfo, sets.data()) !=
        VK_SUCCESS) {
      throw std::runtime_error("Failed to create descriptor set!");
    };
  };

private:
  DPoolVk() { PC_PRINT("CREATED", TagType::Constr, "DPoolVk.h") }
  ~DPoolVk() { PC_PRINT("DESTROYED", TagType::Destr, "DPoolVk.h") }

  void Create(VkDescriptorPoolSize *poolSizes, uint32_t poolSizesCount,
              uint32_t maxSets);
  void CleanUp();

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
  template <DescriptorPools T> DPoolVk &GetPool();
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
