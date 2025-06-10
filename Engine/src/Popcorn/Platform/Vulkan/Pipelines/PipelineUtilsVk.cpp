#include "PipelineUtilsVk.h"
#include "GlobalMacros.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

std::vector<VkDynamicState> PipelineUtilsVk::s_dynamicStatesDefault{
    VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

void PipelineUtilsVk::GetDefaultGfxPipelineState(
    const BufferDefs::Layout &vertexBufferLayout,
    GfxPipelineState &pipelineState,
    std::vector<VkPipelineColorBlendAttachmentState> &colorBlendAttachments) {
  PipelineUtilsVk::GetDefaultDynamicState(pipelineState.dynamicState);
  PipelineUtilsVk::GetDefaultVertexInputState(pipelineState.vertexInputState);
  PipelineUtilsVk::GetDefaultInputAssemblyState(
      pipelineState.inputAssemblyState);
  PipelineUtilsVk::GetDefaultViewportState(pipelineState.viewportState);
  PipelineUtilsVk::GetDefaultRasterizationState(
      pipelineState.rasterizationState);
  PipelineUtilsVk::GetDefaultMultisampleState(pipelineState.multisampleState);
  PipelineUtilsVk::GetDefaultDepthStencilState(
      pipelineState.depthStencilState); // Disabled

  PipelineUtilsVk::GetDefaultColorBlendingState(
      pipelineState.colorBlendState, colorBlendAttachments); // Disabled
  PipelineUtilsVk::GetDefaultPipelineLayoutCreateInfo(
      pipelineState.pipelineLayout); // Disabled
};

void PipelineUtilsVk::GetDefaultInputAssemblyState(
    VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState) {
  inputAssemblyState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyState.primitiveRestartEnable = VK_FALSE;
};

void PipelineUtilsVk::GetDefaultViewportState(
    VkPipelineViewportStateCreateInfo &viewportState) {
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;
};

// FOR DYNAMIC VIEWPORT & SCISSOR STATE DURING DRAW CMD
void PipelineUtilsVk::GetDefaultViewportAndScissorState(
    VkViewport &viewport, VkRect2D &scissor,
    const VkExtent2D &swapchainExtent) {
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

void PipelineUtilsVk::GetDefaultMultisampleState(
    VkPipelineMultisampleStateCreateInfo &multisampleState) {
  multisampleState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleState.sampleShadingEnable = VK_FALSE;
  multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleState.minSampleShading = 1.0f;          // Optional
  multisampleState.pSampleMask = nullptr;            // Optional
  multisampleState.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampleState.alphaToOneEnable = VK_FALSE;      // Optional
};

void PipelineUtilsVk::GetDefaultDepthStencilState(
    VkPipelineDepthStencilStateCreateInfo &depthStencilState) {
  // TODO: Depth & stencil testing

  depthStencilState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilState.depthTestEnable = VK_FALSE; // Disable depth testing
  depthStencilState.depthWriteEnable = VK_FALSE;
};

void PipelineUtilsVk::GetDefaultRasterizationState(
    VkPipelineRasterizationStateCreateInfo &rasterizationState) {
  rasterizationState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationState.depthClampEnable = VK_FALSE;
  rasterizationState.rasterizerDiscardEnable = VK_FALSE;
  rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationState.lineWidth = 1.0f;
  rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
  // rasterizationState.cullMode = VK_CULL_MODE_NONE;
  rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

  rasterizationState.depthBiasEnable = VK_FALSE;
  rasterizationState.depthBiasConstantFactor = 0.0f; // Optional
  rasterizationState.depthBiasClamp = 0.0f;          // Optional
  rasterizationState.depthBiasSlopeFactor = 0.0f;    // Optional
};

void PipelineUtilsVk::GetDefaultDynamicState(
    VkPipelineDynamicStateCreateInfo &dynamicState) {
  // TODO: Make this function flexible to handle other dynamic states
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount =
      static_cast<uint32_t>(s_dynamicStatesDefault.size());
  dynamicState.pDynamicStates = s_dynamicStatesDefault.data();
};

void PipelineUtilsVk::GetDefaultVertexInputState(
    VkPipelineVertexInputStateCreateInfo &vertexInputInfo) {
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
};

void PipelineUtilsVk::GetDefaultColorBlendingState(
    VkPipelineColorBlendStateCreateInfo &colorBlendState,
    std::vector<VkPipelineColorBlendAttachmentState> &colorBlendAttachments) {
  for (uint32_t i = 0; i < colorBlendAttachments.size(); ++i) {
    colorBlendAttachments[i].colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachments[i].blendEnable = VK_FALSE;
    colorBlendAttachments[i].srcColorBlendFactor =
        VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachments[i].dstColorBlendFactor =
        VK_BLEND_FACTOR_ZERO;                                // Optional
    colorBlendAttachments[i].colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachments[i].srcAlphaBlendFactor =
        VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachments[i].dstAlphaBlendFactor =
        VK_BLEND_FACTOR_ZERO;                                // Optional
    colorBlendAttachments[i].alphaBlendOp = VK_BLEND_OP_ADD; // Optional
  }

  colorBlendState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendState.logicOpEnable = VK_FALSE;
  colorBlendState.logicOp = VK_LOGIC_OP_COPY; // Optional
  colorBlendState.attachmentCount = colorBlendAttachments.size();
  colorBlendState.pAttachments = colorBlendAttachments.data();
  colorBlendState.blendConstants[0] = 0.0f; // Optional
  colorBlendState.blendConstants[1] = 0.0f; // Optional
  colorBlendState.blendConstants[2] = 0.0f; // Optional
  colorBlendState.blendConstants[3] = 0.0f; // Optional
};

void PipelineUtilsVk::GetDefaultPipelineLayoutCreateInfo(
    VkPipelineLayoutCreateInfo &pipelineLayoutCreateInfo) {
  pipelineLayoutCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutCreateInfo.setLayoutCount = 0;            // Optional
  pipelineLayoutCreateInfo.pSetLayouts = nullptr;         // Optional
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;    // Optional
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
