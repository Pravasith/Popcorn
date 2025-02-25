#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

std::vector<VkPipelineShaderStageCreateInfo> GfxPipelineVk::CreateShaderStages(
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

  return shaderStages;
};

void GfxPipelineVk::GetDefaultInputAssemblyState(
    VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState) const {
  inputAssemblyState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyState.primitiveRestartEnable = VK_FALSE;
};

void GfxPipelineVk::GetDefaultViewportState(
    VkPipelineViewportStateCreateInfo &viewportState) const {
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;
};

// FOR DYNAMIC VIEWPORT & SCISSOR STATE DURING DRAW CMD
void GfxPipelineVk::GetDefaultViewportAndScissorState(
    VkViewport &viewport, VkRect2D &scissor,
    const VkExtent2D &swapchainExtent) const {
  // Viewport (acts as a transformation)
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swapchainExtent.width;
  viewport.height = (float)swapchainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // Scissor(acts as a filter) VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapchainExtent;
};

void GfxPipelineVk::GetDefaultMultisampleState(
    VkPipelineMultisampleStateCreateInfo &multisampleState) const {
  multisampleState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleState.sampleShadingEnable = VK_FALSE;
  multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleState.minSampleShading = 1.0f;          // Optional
  multisampleState.pSampleMask = nullptr;            // Optional
  multisampleState.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampleState.alphaToOneEnable = VK_FALSE;      // Optional
};

void GfxPipelineVk::GetDefaultDepthStencilState(
    VkPipelineDepthStencilStateCreateInfo &depthStencilState) const {
  // TODO: Depth & stencil testing
};

void GfxPipelineVk::GetDefaultRasterizationState(
    VkPipelineRasterizationStateCreateInfo &rasterizationState) const {
  rasterizationState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationState.depthClampEnable = VK_FALSE;
  rasterizationState.rasterizerDiscardEnable = VK_FALSE;
  rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationState.lineWidth = 1.0f;
  rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;

  rasterizationState.depthBiasEnable = VK_FALSE;
  rasterizationState.depthBiasConstantFactor = 0.0f; // Optional
  rasterizationState.depthBiasClamp = 0.0f;          // Optional
  rasterizationState.depthBiasSlopeFactor = 0.0f;    // Optional
};

void GfxPipelineVk::GetDefaultDynamicState(
    VkPipelineDynamicStateCreateInfo &dynamicState) const {
  // TODO: Make this function flexible to handle other dynamic states
  //
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount =
      static_cast<uint32_t>(m_dynamicStatesDefault.size());
  dynamicState.pDynamicStates = m_dynamicStatesDefault.data();
};

void GfxPipelineVk::GetDefaultVertexInputState(
    VkPipelineVertexInputStateCreateInfo &vertexInputInfo) const {
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
};

void GfxPipelineVk::GetDefaultColorBlendingState(
    VkPipelineColorBlendStateCreateInfo &colorBlending) {

  // Per framebuffer
  // VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  m_colorBlendAttachmentDefault.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  m_colorBlendAttachmentDefault.blendEnable = VK_FALSE;
  m_colorBlendAttachmentDefault.srcColorBlendFactor =
      VK_BLEND_FACTOR_ONE; // Optional
  m_colorBlendAttachmentDefault.dstColorBlendFactor =
      VK_BLEND_FACTOR_ZERO;                                     // Optional
  m_colorBlendAttachmentDefault.colorBlendOp = VK_BLEND_OP_ADD; // Optional
  m_colorBlendAttachmentDefault.srcAlphaBlendFactor =
      VK_BLEND_FACTOR_ONE; // Optional
  m_colorBlendAttachmentDefault.dstAlphaBlendFactor =
      VK_BLEND_FACTOR_ZERO;                                     // Optional
  m_colorBlendAttachmentDefault.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &m_colorBlendAttachmentDefault;
  colorBlending.blendConstants[0] = 0.0f; // Optional
  colorBlending.blendConstants[1] = 0.0f; // Optional
  colorBlending.blendConstants[2] = 0.0f; // Optional
  colorBlending.blendConstants[3] = 0.0f; // Optional
};

void GfxPipelineVk::GetDefaultPipelineLayoutCreateInfo(
    VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) const {
  pipelineLayoutCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = 0;            // Optional
  pipelineLayoutCreateInfo.pSetLayouts = nullptr;         // Optional
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;    // Optional
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional
};

void GfxPipelineVk::SetPipelineLayout(
    const VkDevice &device,
    const VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) {
  PC_VK_NULL_CHECK(device)

  if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr,
                             &m_pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
};

void GfxPipelineVk::DestroyPipelineLayout(const VkDevice &device) {
  vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
