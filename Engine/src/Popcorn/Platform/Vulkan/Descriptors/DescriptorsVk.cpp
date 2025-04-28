#include "DescriptorsVk.h"
#include "ContextVk.h"
#include "DeviceVk.h"
#include "GlobalMacros.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// -------------------------------------------------------------------------
// --- DESCRIPTOR SET LAYOUTS ----------------------------------------------
//
DescriptorSetLayoutsVk *DescriptorSetLayoutsVk::s_instance = nullptr;

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

VkDescriptorSetLayout DescriptorSetLayoutsVk::CreateDescriptorSetLayout(
    const std::vector<VkDescriptorSetLayoutBinding> &bindings) {
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = bindings.size();
  layoutInfo.pBindings = bindings.data();

  VkDescriptorSetLayout descriptorSetLayout{};

  auto &device = DeviceVk::Get()->GetDevice();
  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  return descriptorSetLayout;
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

void DescriptorSetLayoutsVk::GetDefaultDescriptorSetLayoutBindingState(
    VkDescriptorSetLayoutBinding &layoutBinding) {
  layoutBinding.binding = 0;
  layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layoutBinding.descriptorCount = 1;
  layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layoutBinding.pImmutableSamplers = nullptr; // Optional
};

void DescriptorSetLayoutsVk::CleanUp() {
  auto &device = DeviceVk::Get()->GetDevice();

  for (auto &[_, layout] : m_layoutCache) {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
  }

  m_layoutCache.clear();
}

//
//
// -------------------------------------------------------------------------
// --- DESCRIPTOR SETS -----------------------------------------------------
//
// void DescriptorSetsVk::AllocateDescriptorSets(
//     const VkDescriptorSetAllocateInfo &allocInfo,
//     std::vector<VkDescriptorSet> &descriptorSets) {
//
//   auto &device = DeviceVk::Get()->GetDevice();
//   constexpr auto maxFramesInFlight = ContextVk::MAX_FRAMES_IN_FLIGHT;
//   descriptorSets.resize(maxFramesInFlight);
//
//   if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) !=
//       VK_SUCCESS) {
//     throw std::runtime_error("failed to allocate global descriptor sets!");
//   }
// };
//
// void DescriptorSetsVk::GetDefaultDescriptorSetAllocateState(
//     const std::vector<VkDescriptorSetLayout> &dSetLayouts,
//     const VkDescriptorPool &pool, VkDescriptorSetAllocateInfo &allocInfo) {
//   constexpr auto maxFramesInFlight = ContextVk::MAX_FRAMES_IN_FLIGHT;
//   allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//   allocInfo.descriptorPool = pool;
//   allocInfo.descriptorSetCount = static_cast<uint32_t>(dSetLayouts.size());
//   allocInfo.pSetLayouts = dSetLayouts.data();
// };

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
