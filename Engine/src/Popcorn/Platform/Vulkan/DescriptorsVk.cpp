#include "DescriptorsVk.h"
#include "GlobalMacros.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

size_t DescriptorSetLayoutsVk::HashLayoutBindings(
    const std::vector<VkDescriptorSetLayoutBinding> &bindings) {
  size_t hash = 0;

  for (const auto &binding : bindings) {
    size_t bindingHash = 0;
    bindingHash ^= std::hash<uint32_t>()(binding.binding);
    bindingHash ^= std::hash<VkDescriptorType>()(binding.descriptorType) << 1;
    bindingHash ^= std::hash<uint32_t>()(binding.descriptorCount) << 2;
    bindingHash ^= std::hash<VkShaderStageFlags>()(binding.stageFlags) << 3;

    // Mixing function (to add more randomness)
    hash ^= bindingHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
  };

  return hash;
};

VkDescriptorSetLayout &DescriptorSetLayoutsVk::GetLayout(
    const std::vector<VkDescriptorSetLayoutBinding> &bindings) {

  size_t bindingsHash = HashLayoutBindings(bindings);

  if (m_layoutCache.find(bindingsHash) == m_layoutCache.end()) {
    auto layout = CreateDescriptorSetLayout(bindings);
    if (layout == VK_NULL_HANDLE) {
      throw std::runtime_error("Failed to create descriptor set layout");
    }

    m_layoutCache[bindingsHash] = layout;
  };

  return m_layoutCache[bindingsHash];
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
