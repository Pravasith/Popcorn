#include "ContextVk.h"
#include "DescriptorLayoutsVk.h"
#include "GlobalMacros.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

DescriptorLayoutsVk *DescriptorLayoutsVk::s_instance = nullptr;

//
//
//
//
//
// --- DESCRIPTOR LAYOUTS ------------------------------------------------------
// --- DESCRIPTOR LAYOUTS ------------------------------------------------------
// --- DESCRIPTOR LAYOUTS ------------------------------------------------------
//
template <>
VkDescriptorSetLayout &
DescriptorLayoutsVk::GetLayout<DescriptorSets::CameraSet>() {
  if (!m_layouts[DescriptorSets::CameraSet]) {
    VkDescriptorSetLayoutBinding cameraBinding;
    cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraBinding.binding = 0;
    cameraBinding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    cameraBinding.descriptorCount = 1;
    cameraBinding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> cameraBindings{cameraBinding};

    m_layouts[DescriptorSets::CameraSet] =
        ContextVk::DescriptorSetLayouts()->GetLayout(cameraBindings);

    if (m_layouts[DescriptorSets::CameraSet] == VK_NULL_HANDLE) {
      throw std::runtime_error("Camera layout couldn't be created");
    };
  };

  return m_layouts[DescriptorSets::CameraSet];
};

template <>
VkDescriptorSetLayout &
DescriptorLayoutsVk::GetLayout<DescriptorSets::GameObjectSet>() {
  if (!m_layouts[DescriptorSets::GameObjectSet]) {
    VkDescriptorSetLayoutBinding gameObjectBinding;
    gameObjectBinding.descriptorType =
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    gameObjectBinding.binding = 0;
    gameObjectBinding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    gameObjectBinding.descriptorCount = 1;
    gameObjectBinding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> gameObjectBindings{
        gameObjectBinding};

    m_layouts[DescriptorSets::GameObjectSet] =
        ContextVk::DescriptorSetLayouts()->GetLayout(gameObjectBindings);

    if (m_layouts[DescriptorSets::GameObjectSet] == VK_NULL_HANDLE) {
      throw std::runtime_error("GameObject layout couldn't be created");
    };
  };

  return m_layouts[DescriptorSets::GameObjectSet];
};

template <>
VkDescriptorSetLayout &
DescriptorLayoutsVk::GetLayout<DescriptorSets::BasicMatSet>() {
  if (!m_layouts[DescriptorSets::BasicMatSet]) {
    VkDescriptorSetLayoutBinding basicMatBinding;
    basicMatBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    basicMatBinding.binding = 0;
    basicMatBinding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    basicMatBinding.descriptorCount = 1;
    basicMatBinding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> basicMatBindings{basicMatBinding};

    m_layouts[DescriptorSets::BasicMatSet] =
        ContextVk::DescriptorSetLayouts()->GetLayout(basicMatBindings);

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
    VkDescriptorSetLayoutBinding pbrMatBinding;
    pbrMatBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    pbrMatBinding.binding = 0;
    pbrMatBinding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pbrMatBinding.descriptorCount = 1;
    pbrMatBinding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> pbrMatBindings{pbrMatBinding};

    m_layouts[DescriptorSets::PbrMatSet] =
        ContextVk::DescriptorSetLayouts()->GetLayout(pbrMatBindings);

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
    VkDescriptorSetLayoutBinding lightsBinding;
    lightsBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    lightsBinding.binding = 0;
    lightsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    lightsBinding.descriptorCount = 1;
    lightsBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding albedoBuffer;
    albedoBuffer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    albedoBuffer.binding = 1;
    albedoBuffer.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    albedoBuffer.descriptorCount = 1;
    albedoBuffer.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding depthBuffer;
    depthBuffer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    depthBuffer.binding = 2;
    depthBuffer.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    depthBuffer.descriptorCount = 1;
    depthBuffer.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding normalsBuffer;
    normalsBuffer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    normalsBuffer.binding = 3;
    normalsBuffer.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    normalsBuffer.descriptorCount = 1;
    normalsBuffer.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> lightsBindings{
        lightsBinding, albedoBuffer, depthBuffer, normalsBuffer};

    m_layouts[DescriptorSets::LightingSet] =
        ContextVk::DescriptorSetLayouts()->GetLayout(lightsBindings);

    if (m_layouts[DescriptorSets::LightingSet] == VK_NULL_HANDLE) {
      throw std::runtime_error("Lighting layout couldn't be created");
    };
  };

  return m_layouts[DescriptorSets::LightingSet];
};

template <>
VkDescriptorSetLayout &
DescriptorLayoutsVk::GetLayout<DescriptorSets::CompositeSet>() {
  if (!m_layouts[DescriptorSets::CompositeSet]) {
    VkDescriptorSetLayoutBinding lightsBuffer;
    lightsBuffer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    lightsBuffer.binding = 0;
    lightsBuffer.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    lightsBuffer.descriptorCount = 1;
    lightsBuffer.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> compositeBindings{lightsBuffer};

    m_layouts[DescriptorSets::CompositeSet] =
        ContextVk::DescriptorSetLayouts()->GetLayout(compositeBindings);

    if (m_layouts[DescriptorSets::CompositeSet] == VK_NULL_HANDLE) {
      throw std::runtime_error("Composite layout couldn't be created");
    };
  };

  return m_layouts[DescriptorSets::CompositeSet];
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
