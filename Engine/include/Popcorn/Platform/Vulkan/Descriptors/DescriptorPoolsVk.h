#pragma once

#include "DescriptorFactoryVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class DescriptorPools { GBufferPool = 1, LightingPool, CompositePool };

class DPoolVk {
public:
  DPoolVk() { PC_PRINT("CREATED", TagType::Constr, "DPoolVk.h") }
  ~DPoolVk() { PC_PRINT("DESTROYED", TagType::Destr, "DPoolVk.h") }

  template <DescriptorSets T> VkDescriptorSet &AllocateDescriptorSet() {
#if PC_DEBUG
    if (++m_setsAllocated >= m_maxSets) {
      PC_ERROR("Pool reached maximum set capacity of " << m_maxSets << ".",
               "DPoolVk");
    }

#endif
  };

  void AllocateDescriptorSets();
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
