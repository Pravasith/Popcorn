#include "DescriptorLayoutsVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

DescriptorLayoutsVk *DescriptorLayoutsVk::s_instance = nullptr;

template <>
VkDescriptorSetLayout &
DescriptorLayoutsVk::GetLayout<DescriptorSets::CameraSet>() {
  if (!m_layouts[DescriptorSets::CameraSet]) {
    VkDescriptorSetLayoutBinding binding{};
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    binding.binding = 0;
    binding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT |  // Gbuffer pass vertex shader
        VK_SHADER_STAGE_FRAGMENT_BIT; // Light pass frag shader
    binding.descriptorCount = 1;
    binding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> cameraBindings{binding};
    m_layouts[DescriptorSets::CameraSet] = GetCachedLayout(cameraBindings);

    if (m_layouts[DescriptorSets::CameraSet] == VK_NULL_HANDLE) {
      throw std::runtime_error("Camera layout couldn't be created");
    };
  };

  return m_layouts[DescriptorSets::CameraSet];
};

template <>
VkDescriptorSetLayout &
DescriptorLayoutsVk::GetLayout<DescriptorSets::SubmeshSet>() {
  if (!m_layouts[DescriptorSets::SubmeshSet]) {
    VkDescriptorSetLayoutBinding binding{};
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    binding.binding = 0;
    binding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.descriptorCount = 1;
    binding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> submeshBindings{binding};
    m_layouts[DescriptorSets::SubmeshSet] = GetCachedLayout(submeshBindings);

    if (m_layouts[DescriptorSets::SubmeshSet] == VK_NULL_HANDLE) {
      throw std::runtime_error("GameObject layout couldn't be created");
    };
  };

  return m_layouts[DescriptorSets::SubmeshSet];
};

template <>
VkDescriptorSetLayout &
DescriptorLayoutsVk::GetLayout<DescriptorSets::BasicMatSet>() {
  if (!m_layouts[DescriptorSets::BasicMatSet]) {
    VkDescriptorSetLayoutBinding binding{};
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    binding.binding = 0;
    binding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.descriptorCount = 1;
    binding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> basicMatBindings{binding};

    m_layouts[DescriptorSets::BasicMatSet] = GetCachedLayout(basicMatBindings);

    if (m_layouts[DescriptorSets::BasicMatSet] == VK_NULL_HANDLE) {
      throw std::runtime_error("BasicMat layout couldn't be created");
    };
  };

  return m_layouts[DescriptorSets::BasicMatSet];
};

template <>
VkDescriptorSetLayout &
DescriptorLayoutsVk::GetLayout<DescriptorSets::PbrMatSet>() {
  if (!m_layouts[DescriptorSets::PbrMatSet]) {
    VkDescriptorSetLayoutBinding binding{};
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    binding.binding = 0;
    binding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.descriptorCount = 1;
    binding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> pbrMatBindings{binding};

    m_layouts[DescriptorSets::PbrMatSet] = GetCachedLayout(pbrMatBindings);

    if (m_layouts[DescriptorSets::PbrMatSet] == VK_NULL_HANDLE) {
      throw std::runtime_error("PbrMat layout couldn't be created");
    };
  };

  return m_layouts[DescriptorSets::PbrMatSet];
};

template <>
VkDescriptorSetLayout &
DescriptorLayoutsVk::GetLayout<DescriptorSets::LightingSet>() {
  if (!m_layouts[DescriptorSets::LightingSet]) {
    VkDescriptorSetLayoutBinding lightsBinding{};
    lightsBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    lightsBinding.binding = 0;
    lightsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    lightsBinding.descriptorCount = 1;
    lightsBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding albedoBuffer{};
    albedoBuffer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    albedoBuffer.binding = 1;
    albedoBuffer.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    albedoBuffer.descriptorCount = 1;
    albedoBuffer.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding depthBuffer{};
    depthBuffer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    depthBuffer.binding = 2;
    depthBuffer.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    depthBuffer.descriptorCount = 1;
    depthBuffer.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding normalsBuffer{};
    normalsBuffer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    normalsBuffer.binding = 3;
    normalsBuffer.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    normalsBuffer.descriptorCount = 1;
    normalsBuffer.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> lightsBindings{
        lightsBinding, albedoBuffer, depthBuffer, normalsBuffer};

    m_layouts[DescriptorSets::LightingSet] = GetCachedLayout(lightsBindings);

    if (m_layouts[DescriptorSets::LightingSet] == VK_NULL_HANDLE) {
      throw std::runtime_error("Lighting layout couldn't be created");
    };
  };

  return m_layouts[DescriptorSets::LightingSet];
};

template <>
VkDescriptorSetLayout &
DescriptorLayoutsVk::GetLayout<DescriptorSets::PresentSet>() {
  if (!m_layouts[DescriptorSets::PresentSet]) {
    VkDescriptorSetLayoutBinding lightsBuffer;
    lightsBuffer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    lightsBuffer.binding = 0;
    lightsBuffer.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    lightsBuffer.descriptorCount = 1;
    lightsBuffer.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> compositeBindings{lightsBuffer};

    m_layouts[DescriptorSets::PresentSet] = GetCachedLayout(compositeBindings);

    if (m_layouts[DescriptorSets::PresentSet] == VK_NULL_HANDLE) {
      throw std::runtime_error("Composite layout couldn't be created");
    };
  };

  return m_layouts[DescriptorSets::PresentSet];
};

size_t DescriptorLayoutsVk::HashLayoutBindings(
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

VkDescriptorSetLayout DescriptorLayoutsVk::CreateDescriptorSetLayout(
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

VkDescriptorSetLayout &DescriptorLayoutsVk::GetCachedLayout(
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

void DescriptorLayoutsVk::CleanUp() {
  m_layouts.clear();

  for (auto &[_, layout] : m_layoutCache) {
    vkDestroyDescriptorSetLayout(ContextVk::Device()->GetDevice(), layout,
                                 nullptr);
  }

  m_layoutCache.clear();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
