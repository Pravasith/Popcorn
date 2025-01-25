#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Sources.h"
#include <stdexcept>
#include <vector>

ENGINE_NAMESPACE_BEGIN

// Gfx::VertexBufferVk::GetAttrDescs<T>();

void GfxPipelineVk::CreateGfxPipeline(const VkDevice &dev,
                                      const VkExtent2D &swpChnExt) {

  auto vertShdr = m_ShaderVk.LoadFile(PC_src_map[VkTriVert]);
  auto fragShdr = m_ShaderVk.LoadFile(PC_src_map[VkTriFrag]);

  VkShaderModule vertShdrMod = CreateShaderModule(vertShdr, dev);
  VkShaderModule fragShdrMod = CreateShaderModule(fragShdr, dev);

  // CREATE PIPELINE STAGE
  // VERTEX
  VkPipelineShaderStageCreateInfo vertShdrStageInfo{};
  vertShdrStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShdrStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShdrStageInfo.module = vertShdrMod;
  vertShdrStageInfo.pName = "main";

  // CREATE PIPELINE STAGE
  // FRAGMENT
  VkPipelineShaderStageCreateInfo fragShdrStageInfo{};
  fragShdrStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShdrStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShdrStageInfo.module = fragShdrMod;
  fragShdrStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShdrStageInfo,
                                                    fragShdrStageInfo};

  // VERTEX BUFFER DATA
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;

  // INPUT ASSEMBLY
  VkPipelineInputAssemblyStateCreateInfo inptAsmInfo{};
  inptAsmInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inptAsmInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inptAsmInfo.primitiveRestartEnable = VK_FALSE;

  // VIEWPORTS AND SCISSORS
  VkViewport vwprt{};
  vwprt.x = 0.0f;
  vwprt.y = 0.0f;
  vwprt.width = (float)swpChnExt.width;
  vwprt.height = (float)swpChnExt.height;
  vwprt.minDepth = 0.0f;
  vwprt.maxDepth = 1.0f;

  VkRect2D scssr{};
  scssr.offset = {0, 0};
  scssr.extent = swpChnExt;

  // DYNAMIC STATES CONFIG OF THE PIPELINE
  std::vector<VkDynamicState> dynStatesVec = {VK_DYNAMIC_STATE_VIEWPORT,
                                              VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynState{};
  dynState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynState.dynamicStateCount = static_cast<uint32_t>(dynStatesVec.size());
  dynState.pDynamicStates = dynStatesVec.data();

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  // RASTERIZER
  VkPipelineRasterizationStateCreateInfo rstrzr{};
  rstrzr.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rstrzr.depthClampEnable = VK_FALSE;
  rstrzr.rasterizerDiscardEnable = VK_FALSE;
  rstrzr.polygonMode = VK_POLYGON_MODE_FILL;
  rstrzr.lineWidth = 1.0f;
  rstrzr.cullMode = VK_CULL_MODE_BACK_BIT;
  rstrzr.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rstrzr.depthBiasEnable = VK_FALSE;
  rstrzr.depthBiasConstantFactor = 0.0f;
  rstrzr.depthBiasClamp = 0.0f;
  rstrzr.depthBiasSlopeFactor = 0.0f;

  // MULTISAMPLING
  VkPipelineMultisampleStateCreateInfo msmpling{};
  msmpling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  msmpling.sampleShadingEnable = VK_FALSE;
  msmpling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  msmpling.minSampleShading = 1.0f;
  msmpling.pSampleMask = nullptr;
  msmpling.alphaToCoverageEnable = VK_FALSE;
  msmpling.alphaToOneEnable = VK_FALSE;

  // DEPTH TESTING - RN IT'S NULLPTR

  // COLOR BLENDING
  VkPipelineColorBlendAttachmentState clrBldAtchmt{};
  clrBldAtchmt.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  clrBldAtchmt.blendEnable = VK_FALSE;
  clrBldAtchmt.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  clrBldAtchmt.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  clrBldAtchmt.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
  clrBldAtchmt.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  clrBldAtchmt.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  clrBldAtchmt.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

  VkPipelineColorBlendStateCreateInfo clrBlding{};
  clrBlding.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  clrBlding.logicOpEnable = VK_FALSE;
  clrBlding.logicOp = VK_LOGIC_OP_COPY; // Optional
  clrBlding.attachmentCount = 1;
  clrBlding.pAttachments = &clrBldAtchmt;
  clrBlding.blendConstants[0] = 0.0f; // Optional
  clrBlding.blendConstants[1] = 0.0f; // Optional
  clrBlding.blendConstants[2] = 0.0f; // Optional
  clrBlding.blendConstants[3] = 0.0f; // Optional

  // PIPELINE LAYOUT
  VkPipelineLayoutCreateInfo pplnLytInfo{};
  pplnLytInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pplnLytInfo.setLayoutCount = 0;            // Optional
  pplnLytInfo.pSetLayouts = nullptr;         // Optional
  pplnLytInfo.pushConstantRangeCount = 0;    // Optional
  pplnLytInfo.pPushConstantRanges = nullptr; // Optional

  if (vkCreatePipelineLayout(dev, &pplnLytInfo, nullptr, &m_pipelineLayout) !=
      VK_SUCCESS) {
    std::runtime_error("FAILED TO CREATE PIPELINE LAYOUT!");
  };

  // CREATE GRAPHICS PIPELINE
  VkGraphicsPipelineCreateInfo gfxPlInfo{};
  gfxPlInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  gfxPlInfo.stageCount = 2;
  gfxPlInfo.pStages = shaderStages;
  gfxPlInfo.pVertexInputState = &vertexInputInfo;
  gfxPlInfo.pInputAssemblyState = &inptAsmInfo;
  gfxPlInfo.pViewportState = &viewportState;
  gfxPlInfo.pRasterizationState = &rstrzr;
  gfxPlInfo.pMultisampleState = &msmpling;
  gfxPlInfo.pDepthStencilState = nullptr;
  gfxPlInfo.pColorBlendState = &clrBlding;
  gfxPlInfo.pDynamicState = &dynState;
  gfxPlInfo.layout = m_pipelineLayout;
  gfxPlInfo.renderPass = m_renderPass;
  gfxPlInfo.subpass = 0; // INDEX OF THE SUBPASS
  gfxPlInfo.basePipelineHandle = VK_NULL_HANDLE;
  gfxPlInfo.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1, &gfxPlInfo, nullptr,
                                &m_gfxPipeline) != VK_SUCCESS) {
    std::runtime_error("ERROR: FAILED TO CREATE GFX PIPELINE!");
  };

  vkDestroyShaderModule(dev, vertShdrMod, nullptr);
  vkDestroyShaderModule(dev, fragShdrMod, nullptr);
};

void GfxPipelineVk::CreateRenderPass(const VkFormat &swpChnImgFrmt,
                                     const VkDevice &dev) {
  VkAttachmentDescription clrAtmt{};
  clrAtmt.format = swpChnImgFrmt;
  clrAtmt.samples = VK_SAMPLE_COUNT_1_BIT;
  clrAtmt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  clrAtmt.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  clrAtmt.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  clrAtmt.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  clrAtmt.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  clrAtmt.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // SUBPASSES & ATTACHMENT REFERENCES
  VkAttachmentReference clrAttmtRef{};
  clrAttmtRef.attachment = 0;
  clrAttmtRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription sbPass{};
  sbPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  sbPass.colorAttachmentCount = 1;
  sbPass.pColorAttachments = &clrAttmtRef;

  VkSubpassDependency subpassDep{};
  subpassDep.srcSubpass = VK_SUBPASS_EXTERNAL;
  subpassDep.dstSubpass = 0;
  subpassDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDep.srcAccessMask = 0;
  subpassDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpassDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  // RENDER PASS CREATION
  VkRenderPassCreateInfo rndrPassInfo{};
  rndrPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  rndrPassInfo.attachmentCount = 1;
  rndrPassInfo.pAttachments = &clrAtmt;
  rndrPassInfo.subpassCount = 1;
  rndrPassInfo.pSubpasses = &sbPass;
  rndrPassInfo.dependencyCount = 1;
  rndrPassInfo.pDependencies = &subpassDep;

  if (vkCreateRenderPass(dev, &rndrPassInfo, nullptr, &m_renderPass) !=
      VK_SUCCESS) {
    std::runtime_error("ERROR: RENDER PASS COULDN'T BE CREATED!");
  };
};

VkShaderModule GfxPipelineVk::CreateShaderModule(const std::vector<char> &code,
                                                 const VkDevice &device) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shdrModl;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shdrModl) !=
      VK_SUCCESS) {
    throw std::runtime_error("ERROR: FAILED TO CREATE SHADER MODULE");
  };

  return shdrModl;
};

void GfxPipelineVk::CleanUp(const VkDevice &dev) {
  vkDestroyPipeline(dev, m_gfxPipeline, nullptr);
  vkDestroyPipelineLayout(dev, m_pipelineLayout, nullptr);
  vkDestroyRenderPass(dev, m_renderPass, nullptr);
};

ENGINE_NAMESPACE_END
