#include "DescriptorFactoryVk.h"
#include "DescriptorsVk.h"
#include "GlobalMacros.h"
#include "PipelineFactoryVk.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

DescriptorFactoryVk *DescriptorFactoryVk::s_instance = nullptr;

template <>
void DescriptorFactoryVk::CreateDescriptorSetLayouts<Pipelines::Base>() {
  //
  //  1. Global Descriptors (Set 0) - UBO
  //      - View & projection matrices (mat4 each)
  //  2. Per-Mesh Descriptors (Set 1) - Dynamic UBO
  //      - Model matrix (mat4, dynamically offset for instancing)
  //  3. Material Descriptors (Set 2) - UBO
  //      - Albedo (vec4)
  //      - Roughness (float) + Metalness (float) (+ padding/other packed
  //      fields)
  //
  //  ------------------ OPTIONAL -------------------------------------
  //  4. Texture Descriptors (Set 3) - Combined Image Samplers
  //      - Albedo/Alpha map (R8G8B8A8_UNORM)
  //      - Normal map (optional)
  //      - Roughness/Metalness/AO map (optional, packed channels)
  //

  DescriptorSetLayoutsVk *dSetLayoutsVkStn = DescriptorSetLayoutsVk::Get();

  // This is a global binding at the moment (UBO for MVP transform matrix)
  // Loop through material types & make different layouts

  //
  // Camera bindings & layouts ----------------------------------------
  VkDescriptorSetLayoutBinding cameraBinding{};
  DescriptorSetLayoutsVk::GetDefaultDescriptorSetLayoutBindingState(
      cameraBinding);
  cameraBinding.binding = 0;
  std::vector<VkDescriptorSetLayoutBinding> vertexCameraBindings{cameraBinding};
  auto &cameraLayout = dSetLayoutsVkStn->GetLayout(vertexCameraBindings);
  if (cameraLayout == VK_NULL_HANDLE) {
    PC_ERROR("Failed to create camera binding descriptor set layout!",
             "DescriptorFactoryVk")
    return;
  }
  m_descriptorLayoutMap[Pipelines::Base].push_back(cameraLayout);

  //
  // Mesh bindings & layouts ------------------------------------------
  VkDescriptorSetLayoutBinding meshBinding{};
  DescriptorSetLayoutsVk::GetDefaultDescriptorSetLayoutBindingState(
      meshBinding);
  meshBinding.binding = 0;
  meshBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  std::vector<VkDescriptorSetLayoutBinding> vertexMeshBindings{meshBinding};
  auto &meshLayout = dSetLayoutsVkStn->GetLayout(vertexMeshBindings);
  if (meshLayout == VK_NULL_HANDLE) {
    PC_ERROR("Failed to create mesh binding descriptor set layout!",
             "DescriptorFactoryVk")
    return;
  }
  m_descriptorLayoutMap[Pipelines::Base].push_back(meshLayout);

  //
  // Material bindings & layouts --------------------------------------
  VkDescriptorSetLayoutBinding materialBinding{};
  DescriptorSetLayoutsVk::GetDefaultDescriptorSetLayoutBindingState(
      materialBinding);
  materialBinding.binding = 0;
  std::vector<VkDescriptorSetLayoutBinding> vertexMaterialBindings{
      materialBinding};
  auto &materialLayout = dSetLayoutsVkStn->GetLayout(vertexMaterialBindings);
  if (materialLayout == VK_NULL_HANDLE) {
    PC_ERROR("Failed to create material binding descriptor set layout!",
             "DescriptorFactoryVk")
    return;
  }
  m_descriptorLayoutMap[Pipelines::Base].push_back(materialLayout);
};

template <>
void DescriptorFactoryVk::CreateDescriptorSetLayouts<Pipelines::Deferred>() {};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
