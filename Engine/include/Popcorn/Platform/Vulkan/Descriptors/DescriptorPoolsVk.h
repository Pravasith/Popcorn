#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <unordered_map>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class DescriptorPools { GBufferPool = 1, LightingPool, CompositePool };

// TODO: Make it a growable pool
class DescriptorPoolsVk {

public:
  template <DescriptorPools T> VkDescriptorPool &GetPool();
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
  };
  ~DescriptorPoolsVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "DescriptorPoolsVk.h")
  };

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  DescriptorPoolsVk(const DescriptorPoolsVk &) = delete;
  DescriptorPoolsVk &operator=(const DescriptorPoolsVk &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
  DescriptorPoolsVk(DescriptorPoolsVk &&) = delete;
  DescriptorPoolsVk &operator=(DescriptorPoolsVk &&) = delete;

private:
  static DescriptorPoolsVk *s_instance;
  std::unordered_map<DescriptorPools, VkDescriptorPool> m_pools{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
