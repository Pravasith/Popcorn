#include "PipelineDefsVk.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "BufferObjectsVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "MaterialPipelinesVk.h"
#include "MaterialTypes.h"
#include "PipelineUtilsVk.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// --- BASIC MATERIAL PIPELINE -------------------------------------------------
// --- BASIC MATERIAL PIPELINE -------------------------------------------------
// --- BASIC MATERIAL PIPELINE -------------------------------------------------
//
void BasicMatPipelineVk::Create(const BufferDefs::Layout &vertexBufferLayout,
                                const VkRenderPass &renderPass) {
  const VkDevice &device = ContextVk::Device()->GetDevice();

  const Buffer &vertShaderBuffer =
      Material<MaterialTypes::BasicMat>::GetShader(ShaderStages::VertexBit);
  const Buffer &fragShaderBuffer =
      Material<MaterialTypes::BasicMat>::GetShader(ShaderStages::FragmentBit);

  auto vertShaderModule = PC_CreateShaderModule(device, vertShaderBuffer);
  auto fragShaderModule = PC_CreateShaderModule(device, fragShaderBuffer);

  std::forward_list<VkShaderModule> shaderModules = {vertShaderModule,
                                                     fragShaderModule};

  SetShaderStagesMask(ShaderStages::VertexBit | ShaderStages::FragmentBit);
  CreateShaderStageCreateInfos(shaderModules);

  VkVertexInputBindingDescription bindingDescription{};
  VertexBufferVk::GetDefaultVertexInputBindingDescription(vertexBufferLayout,
                                                          bindingDescription);
  bindingDescription.binding = 0;

  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
  VertexBufferVk::GetDefaultVertexInputAttributeDescriptions(
      vertexBufferLayout, attributeDescriptions);

  std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
  colorBlendAttachments.resize(3);

  GfxPipelineState pipelineState{};
  PipelineUtilsVk::GetDefaultGfxPipelineState(vertexBufferLayout, pipelineState,
                                              colorBlendAttachments);

  pipelineState.vertexInputState.vertexBindingDescriptionCount = 1;
  pipelineState.vertexInputState.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  pipelineState.vertexInputState.pVertexBindingDescriptions =
      &bindingDescription;
  pipelineState.vertexInputState.pVertexAttributeDescriptions =
      attributeDescriptions.data();

  pipelineState.depthStencilState.depthTestEnable = VK_TRUE;
  pipelineState.depthStencilState.depthWriteEnable = VK_TRUE;
  pipelineState.depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
  pipelineState.depthStencilState.depthBoundsTestEnable = VK_FALSE;
  pipelineState.depthStencilState.minDepthBounds = 0.0f;
  pipelineState.depthStencilState.maxDepthBounds = 1.0f;
  pipelineState.depthStencilState.stencilTestEnable = VK_FALSE;
  pipelineState.depthStencilState.front = {};
  pipelineState.depthStencilState.back = {};

  auto *layouts = ContextVk::DescriptorLayouts();
  VkDescriptorSetLayout &cameraLayout =
      layouts->GetLayout<DescriptorSets::CameraSet>();
  VkDescriptorSetLayout &basicMatLayout =
      layouts->GetLayout<DescriptorSets::BasicMatSet>();
  VkDescriptorSetLayout &submeshLayout =
      layouts->GetLayout<DescriptorSets::SubmeshSet>();

  std::array<VkDescriptorSetLayout, 3> dSetLayouts = {
      cameraLayout, basicMatLayout, submeshLayout};

  // Pipeline Layout (descriptor set layout data)
  pipelineState.pipelineLayout.setLayoutCount = dSetLayouts.size();
  pipelineState.pipelineLayout.pSetLayouts = dSetLayouts.data();

  CreatePipelineLayout(device, pipelineState.pipelineLayout);
  CreateVkPipeline(device, pipelineState, renderPass);

  // DESTROY SHADER MODULES
  PC_DestroyShaderModule(device, vertShaderModule);
  PC_DestroyShaderModule(device, fragShaderModule);
};

//
//
//
//
// --- PBR MATERIAL PIPELINE ---------------------------------------------------
// --- PBR MATERIAL PIPELINE ---------------------------------------------------
// --- PBR MATERIAL PIPELINE ---------------------------------------------------
//

void PbrMatPipelineVk::Create(const BufferDefs::Layout &vertexBufferLayout,
                              const VkRenderPass &renderPass) {
  const VkDevice &device = ContextVk::Device()->GetDevice();

  const Buffer &vertShaderBuffer =
      Material<MaterialTypes::PbrMat>::GetShader(ShaderStages::VertexBit);
  const Buffer &fragShaderBuffer =
      Material<MaterialTypes::PbrMat>::GetShader(ShaderStages::FragmentBit);

  auto vertShaderModule = PC_CreateShaderModule(device, vertShaderBuffer);
  auto fragShaderModule = PC_CreateShaderModule(device, fragShaderBuffer);

  std::forward_list<VkShaderModule> shaderModules = {vertShaderModule,
                                                     fragShaderModule};

  SetShaderStagesMask(ShaderStages::VertexBit | ShaderStages::FragmentBit);
  CreateShaderStageCreateInfos(shaderModules);

  VkVertexInputBindingDescription bindingDescription{};
  VertexBufferVk::GetDefaultVertexInputBindingDescription(vertexBufferLayout,
                                                          bindingDescription);
  bindingDescription.binding = 0;

  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
  VertexBufferVk::GetDefaultVertexInputAttributeDescriptions(
      vertexBufferLayout, attributeDescriptions);

  std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
  colorBlendAttachments.resize(3);

  GfxPipelineState pipelineState{};
  PipelineUtilsVk::GetDefaultGfxPipelineState(vertexBufferLayout, pipelineState,
                                              colorBlendAttachments);

  pipelineState.vertexInputState.vertexBindingDescriptionCount = 1;
  pipelineState.vertexInputState.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  pipelineState.vertexInputState.pVertexBindingDescriptions =
      &bindingDescription;
  pipelineState.vertexInputState.pVertexAttributeDescriptions =
      attributeDescriptions.data();

  pipelineState.depthStencilState.depthTestEnable = VK_TRUE;
  pipelineState.depthStencilState.depthWriteEnable = VK_TRUE;
  pipelineState.depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
  pipelineState.depthStencilState.depthBoundsTestEnable = VK_FALSE;
  pipelineState.depthStencilState.minDepthBounds = 0.0f;
  pipelineState.depthStencilState.maxDepthBounds = 1.0f;
  pipelineState.depthStencilState.stencilTestEnable = VK_FALSE;
  pipelineState.depthStencilState.front = {};
  pipelineState.depthStencilState.back = {};

  auto *layouts = ContextVk::DescriptorLayouts();
  VkDescriptorSetLayout &cameraLayout =
      layouts->GetLayout<DescriptorSets::CameraSet>();
  VkDescriptorSetLayout &pbrMatLayout =
      layouts->GetLayout<DescriptorSets::PbrMatSet>();
  VkDescriptorSetLayout &submeshLayout =
      layouts->GetLayout<DescriptorSets::SubmeshSet>();

  std::array<VkDescriptorSetLayout, 3> dSetLayouts = {
      cameraLayout, pbrMatLayout, submeshLayout};

  // Pipeline Layout (descriptor set layout data)
  pipelineState.pipelineLayout.setLayoutCount = dSetLayouts.size();
  pipelineState.pipelineLayout.pSetLayouts = dSetLayouts.data();

  CreatePipelineLayout(device, pipelineState.pipelineLayout);
  CreateVkPipeline(device, pipelineState, renderPass);

  // Destroy shader modules
  PC_DestroyShaderModule(device, vertShaderModule);
  PC_DestroyShaderModule(device, fragShaderModule);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
