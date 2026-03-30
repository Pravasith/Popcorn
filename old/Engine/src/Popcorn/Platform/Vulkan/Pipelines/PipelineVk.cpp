#include "PipelineVk.h"
#include "ContextVk.h"
#include "GlobalMacros.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <>
void PipelineVk<PipelineTypes::GraphicsType>::CreateVkPipeline(
    const VkDevice &device, const PipelineStateType &pipelineCreateInfo,
    const VkRenderPass &renderPass) {

  if (m_pipeline != VK_NULL_HANDLE) {
    PC_WARN("Attempt to create GBufferPipeline when it already exists")
    return;
  };

  GfxPipelineState createInfo = pipelineCreateInfo;
  if (m_shaderStageCreateInfos.size() == 0) {
    PC_WARN("No shader stages set")
  };

  PC_VK_NULL_CHECK(m_pipelineLayout);

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

  pipelineInfo.stageCount = m_shaderStageCreateInfos.size();
  pipelineInfo.pStages = m_shaderStageCreateInfos.data();

  pipelineInfo.pVertexInputState = &createInfo.vertexInputState;
  pipelineInfo.pInputAssemblyState = &createInfo.inputAssemblyState;
  pipelineInfo.pViewportState = &createInfo.viewportState;
  pipelineInfo.pRasterizationState = &createInfo.rasterizationState;
  pipelineInfo.pDepthStencilState = &createInfo.depthStencilState;
  // pipelineInfo.pDepthStencilState = nullptr;
  pipelineInfo.pMultisampleState = &createInfo.multisampleState;
  pipelineInfo.pColorBlendState = &createInfo.colorBlendState;
  pipelineInfo.pDynamicState = &createInfo.dynamicState;

  pipelineInfo.layout = m_pipelineLayout;

  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;

  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex = -1;              // Optional

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &m_pipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

template void PipelineVk<PipelineTypes::GraphicsType>::SetShaderStagesMask(
    int enabledShaderStagesMask);

template void PipelineVk<PipelineTypes::ComputeType>::SetShaderStagesMask(
    int enabledShaderStagesMask);

template void PipelineVk<PipelineTypes::RaytracingType>::SetShaderStagesMask(
    int enabledShaderStagesMask);

template <PipelineTypes T>
void PipelineVk<T>::SetShaderStagesMask(int enabledShaderStagesMask) {
  if constexpr (T == PipelineTypes::GraphicsType) {
    bool invalidGraphicsPipeline =
        (enabledShaderStagesMask &
         (ShaderStages::VertexBit | ShaderStages::FragmentBit)) !=
        (ShaderStages::VertexBit | ShaderStages::FragmentBit);
    if (invalidGraphicsPipeline)
      PC_ERROR(
          "Either vertex shader or fragment shader or both are not enabled",
          "PipelineVk");
  } else if constexpr (T == PipelineTypes::ComputeType) {
    bool invalidComputePipeline =
        !(enabledShaderStagesMask & ShaderStages::ComputeBit);
    if (invalidComputePipeline)
      PC_ERROR("Compute shader is not enabled", "PipelineVk");
  } else if constexpr (T == PipelineTypes::RaytracingType) {
    // TODO: Fill it out
  }

  m_enabledShaderStagesMask = enabledShaderStagesMask;
}

template <>
void PipelineVk<PipelineTypes::GraphicsType>::CreateShaderStageCreateInfos(
    std::forward_list<VkShaderModule> &shaderModules) {

  std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};

  if (shaderModules.empty()) {
    throw std::runtime_error(
        "Not enough shader modules provided for the enabled stages.");
  }

  //
  // VERTEX STAGE -----------------------------------------------------
  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = shaderModules.front();
  shaderModules.pop_front();
  vertShaderStageInfo.pName = "main";
  shaderStages.emplace_back(vertShaderStageInfo);

  //
  // TESSELATION STAGE ------------------------------------------------
  if (m_enabledShaderStagesMask & ShaderStages::TesselationBit) {
    // TODO: FILL THIS OUT
    // shaderStages.emplace_back(tessShaderStageInfo);
  };

  //
  // GEOMETRY STAGE ---------------------------------------------------
  if (m_enabledShaderStagesMask & ShaderStages::GeometryBit) {
    // TODO: FILL THIS OUT
    // shaderStages.emplace_back(geometryShaderStageInfo);
  };

  //
  // FRAGMENT STAGE ---------------------------------------------------
  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = shaderModules.front();
  shaderModules.pop_front();
  fragShaderStageInfo.pName = "main";
  shaderStages.emplace_back(fragShaderStageInfo);

  m_shaderStageCreateInfos = std::move(shaderStages);
};

template <>
void PipelineVk<PipelineTypes::GraphicsType>::CreatePipelineLayout(
    const VkDevice &device,
    const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) {
  PC_VK_NULL_CHECK(device)
  if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr,
                             &m_pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
};

template <>
void PipelineVk<PipelineTypes::GraphicsType>::DestroyPipelineLayout(
    const VkDevice &device) {
  vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
};

template <> void PipelineVk<PipelineTypes::GraphicsType>::Destroy() {
  const auto &device = ContextVk::Device()->GetDevice();

  PC_VK_NULL_CHECK(m_pipeline)
  DestroyPipelineLayout(device);

  vkDestroyPipeline(device, m_pipeline, nullptr);
  m_pipeline = VK_NULL_HANDLE;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
