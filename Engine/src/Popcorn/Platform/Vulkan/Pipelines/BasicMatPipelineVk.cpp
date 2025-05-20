#include "BasicMatPipelineVk.h"
#include "BufferObjectsVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "MaterialTypes.h"
#include "PipelineUtilsVk.h"
#include "Shader.h"
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void BasicMatPipelineVk::Create(const BufferDefs::Layout &vertexBufferLayout) {
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

  GfxPipelineState pipelineState{};
  PipelineUtilsVk::GetDefaultDynamicState(pipelineState.dynamicState);
  PipelineUtilsVk::GetDefaultVertexInputState(pipelineState.vertexInputState);

  // Vertex input descriptions
  VkVertexInputBindingDescription bindingDescription{};
  VertexBufferVk::GetDefaultVertexInputBindingDescription(vertexBufferLayout,
                                                          bindingDescription);
  bindingDescription.binding = 0;

  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
  VertexBufferVk::GetDefaultVertexInputAttributeDescriptions(
      vertexBufferLayout, attributeDescriptions);

  pipelineState.vertexInputState.vertexBindingDescriptionCount = 1;
  pipelineState.vertexInputState.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  pipelineState.vertexInputState.pVertexBindingDescriptions =
      &bindingDescription;
  pipelineState.vertexInputState.pVertexAttributeDescriptions =
      attributeDescriptions.data();

  PipelineUtilsVk::GetDefaultInputAssemblyState(
      pipelineState.inputAssemblyState);
  PipelineUtilsVk::GetDefaultViewportState(pipelineState.viewportState);
  PipelineUtilsVk::GetDefaultRasterizationState(
      pipelineState.rasterizationState);
  PipelineUtilsVk::GetDefaultMultisampleState(pipelineState.multisampleState);
  PipelineUtilsVk::GetDefaultDepthStencilState(pipelineState.depthStencilState);
  // pipelineState.depthStencilState.depthTestEnable = VK_TRUE;
  // pipelineState.depthStencilState.depthWriteEnable = VK_TRUE;
  // TODO: Read depth-buffering & implement
  PipelineUtilsVk::GetDefaultColorBlendingState(
      pipelineState.colorBlendState); // Disabled
  PipelineUtilsVk::GetDefaultPipelineLayoutCreateInfo(
      pipelineState.pipelineLayout);

  // TODO: Get descriptor layouts and attach

  // DESTROY SHADER MODULES
  PC_DestroyShaderModule(device, vertShaderModule);
  PC_DestroyShaderModule(device, fragShaderModule);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
