#include "DescriptorFactoryVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

DescriptorFactoryVk *DescriptorFactoryVk::s_instance = nullptr;

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
DescriptorFactoryVk::GetLayout<DescriptorSets::CameraSet>() {
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
DescriptorFactoryVk::GetLayout<DescriptorSets::GameObjectSet>() {
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
DescriptorFactoryVk::GetLayout<DescriptorSets::BasicMatSet>() {
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
DescriptorFactoryVk::GetLayout<DescriptorSets::PbrMatSet>() {
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

//
//
//
//
//
// --- DESCRIPTOR SETS ---------------------------------------------------------
// --- DESCRIPTOR SETS ---------------------------------------------------------
// --- DESCRIPTOR SETS ---------------------------------------------------------
//
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
