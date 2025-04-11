#include "GBufferPipelineVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void GBufferPipelineVk::CreateVkPipeline(
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
  // pipelineInfo.pDepthStencilState = &createInfo.depthStencilState;
  pipelineInfo.pDepthStencilState = nullptr;
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

void GBufferPipelineVk::CleanUp(const VkDevice &device) {
  PC_VK_NULL_CHECK(device)
  PC_VK_NULL_CHECK(m_pipeline)
  DestroyPipelineLayout(device);

  vkDestroyPipeline(device, m_pipeline, nullptr);
  m_pipeline = VK_NULL_HANDLE;
};

void GBufferPipelineVk::CreateShaderStageCreateInfos(
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

void GBufferPipelineVk::CreatePipelineLayout(
    const VkDevice &device,
    const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) {
  PC_VK_NULL_CHECK(device)

  if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr,
                             &m_pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
};

void GBufferPipelineVk::DestroyPipelineLayout(const VkDevice &device) {
  vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
