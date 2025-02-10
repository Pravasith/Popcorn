#include "PipelineVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void PipelineVk::SetShaderStages(ShaderStages enabledShaderStagesMask) {
  PC_PRINT((int)enabledShaderStagesMask, TagType::Print, "PipelineVk")
  // Error check
  switch (type_value) {
  case PipelineTypes::None:
    PC_ERROR("PipelineType not set yet!!", "PipelineVk")
    break;
  case PipelineTypes::GraphicsType:
    if ((enabledShaderStagesMask &
         (ShaderStages::Vertex | ShaderStages::Fragment)) !=
        (ShaderStages::Vertex | ShaderStages::Fragment)) {
      PC_ERROR(
          "Either vertex shader or fragment shader or both are not enabled",
          "PipelineVk")
    };
    break;
  case PipelineTypes::ComputeType:
    if (!(enabledShaderStagesMask & ShaderStages::Compute))
      PC_ERROR("Compute shader is not enabled", "PipelineVk")
    break;
  case PipelineTypes::RaytracingType:
    // TODO: Fill it out
    break;
  }

  m_enabledShaderStagesMask = enabledShaderStagesMask;
};

VkShaderModule PipelineVk::CreateShaderModule(const VkDevice &device,
                                              const Buffer &bytecode) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = bytecode.GetSize();
  createInfo.pCode = bytecode.AsType<uint32_t>();

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}

void PipelineVk::SetDefaultDynamicState(
    VkPipelineDynamicStateCreateInfo &dynamicState) {
  dynamicState = {}; // Provide a default empty state
}

void PipelineVk::SetDefaultVertexInputState(
    VkPipelineVertexInputStateCreateInfo &vertexInputState) {
  vertexInputState = {}; // Provide an empty implementation
}

void PipelineVk::SetDefaultInputAssemblyState(
    VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState) {
  inputAssemblyState = {}; // Provide an empty implementation
}

void PipelineVk::SetDefaultViewportState(
    VkPipelineViewportStateCreateInfo &viewportState,
    const VkExtent2D &extent) {
  viewportState = {}; // Provide an empty implementation
}

void PipelineVk::SetDefaultRasterizationState(
    VkPipelineRasterizationStateCreateInfo &rasterizationState) {
  rasterizationState = {}; // Provide an empty implementation
}

void PipelineVk::SetDefaultMultisampleState(
    VkPipelineMultisampleStateCreateInfo &multisampleState) {
  multisampleState = {}; // Provide an empty implementation
}

void PipelineVk::SetDefaultDepthStencilState(
    VkPipelineDepthStencilStateCreateInfo &depthStencilState) {
  depthStencilState = {}; // Provide an empty implementation
}

std::pair<VkViewport, VkRect2D>
PipelineVk::GetViewportAndScissor(const VkExtent2D &swapchainExtent) const {
  return {}; // Provide default or proper values
}

void PipelineVk::SetDefaultColorBlendingState(
    VkPipelineColorBlendStateCreateInfo &) {
  // Provide an empty implementation
}

void PipelineVk::SetDefaultPipelineLayout(const VkDevice &device) {
  // Provide an empty implementation
}

void PipelineVk::DestroyPipelineLayout(const VkDevice &device) {
  // Provide an empty implementation
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
