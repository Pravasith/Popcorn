#include "PipelineVk.h"
#include "GlobalMacros.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

std::vector<VkDynamicState> PipelineUtils::s_dynamicStatesDefault{
    VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
VkPipelineColorBlendAttachmentState
    PipelineUtils::s_colorBlendAttachmentDefault{};

void PipelineUtils::GetDefaultInputAssemblyState(
    VkPipelineInputAssemblyStateCreateInfo &inputAssemblyState) {
  inputAssemblyState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyState.primitiveRestartEnable = VK_FALSE;
};

void PipelineUtils::GetDefaultViewportState(
    VkPipelineViewportStateCreateInfo &viewportState) {
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;
};

// FOR DYNAMIC VIEWPORT & SCISSOR STATE DURING DRAW CMD
void PipelineUtils::GetDefaultViewportAndScissorState(
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

void PipelineUtils::GetDefaultMultisampleState(
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

void PipelineUtils::GetDefaultDepthStencilState(
    VkPipelineDepthStencilStateCreateInfo &depthStencilState) {
  // TODO: Depth & stencil testing

  depthStencilState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilState.depthTestEnable = VK_FALSE; // Disable depth testing
  depthStencilState.depthWriteEnable = VK_FALSE;
};

void PipelineUtils::GetDefaultRasterizationState(
    VkPipelineRasterizationStateCreateInfo &rasterizationState) {
  rasterizationState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationState.depthClampEnable = VK_FALSE;
  rasterizationState.rasterizerDiscardEnable = VK_FALSE;
  rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationState.lineWidth = 1.0f;
  rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
  // rasterizationState.cullMode = VK_CULL_MODE_NONE;
  rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;

  rasterizationState.depthBiasEnable = VK_FALSE;
  rasterizationState.depthBiasConstantFactor = 0.0f; // Optional
  rasterizationState.depthBiasClamp = 0.0f;          // Optional
  rasterizationState.depthBiasSlopeFactor = 0.0f;    // Optional
};

void PipelineUtils::GetDefaultDynamicState(
    VkPipelineDynamicStateCreateInfo &dynamicState) {
  // TODO: Make this function flexible to handle other dynamic states
  //
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount =
      static_cast<uint32_t>(s_dynamicStatesDefault.size());
  dynamicState.pDynamicStates = s_dynamicStatesDefault.data();
};

void PipelineUtils::GetDefaultVertexInputState(
    VkPipelineVertexInputStateCreateInfo &vertexInputInfo) {
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
};

void PipelineUtils::GetDefaultColorBlendingState(
    VkPipelineColorBlendStateCreateInfo &colorBlending) {

  // Per framebuffer
  // VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  s_colorBlendAttachmentDefault.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  s_colorBlendAttachmentDefault.blendEnable = VK_FALSE;
  s_colorBlendAttachmentDefault.srcColorBlendFactor =
      VK_BLEND_FACTOR_ONE; // Optional
  s_colorBlendAttachmentDefault.dstColorBlendFactor =
      VK_BLEND_FACTOR_ZERO;                                     // Optional
  s_colorBlendAttachmentDefault.colorBlendOp = VK_BLEND_OP_ADD; // Optional
  s_colorBlendAttachmentDefault.srcAlphaBlendFactor =
      VK_BLEND_FACTOR_ONE; // Optional
  s_colorBlendAttachmentDefault.dstAlphaBlendFactor =
      VK_BLEND_FACTOR_ZERO;                                     // Optional
  s_colorBlendAttachmentDefault.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &s_colorBlendAttachmentDefault;
  colorBlending.blendConstants[0] = 0.0f; // Optional
  colorBlending.blendConstants[1] = 0.0f; // Optional
  colorBlending.blendConstants[2] = 0.0f; // Optional
  colorBlending.blendConstants[3] = 0.0f; // Optional
};

void PipelineUtils::GetDefaultPipelineLayoutCreateInfo(
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
