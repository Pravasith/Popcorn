#include "GfxPipelineVk.h"
#include "Global_Macros.h"
#include "Sources.h"
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN

void GfxPipelineVk::CreateGfxPipeline(const VkDevice &dev,
                                      const VkExtent2D &swpChnExt) {
  auto vertShdr = m_ShdrVk.LoadFile(PC_src_map[VkTriVert]);
  auto fragShdr = m_ShdrVk.LoadFile(PC_src_map[VkTriFrag]);

  VkShaderModule vertShdrMod = CreateShaderModule(vertShdr, dev);
  VkShaderModule fragShdrMod = CreateShaderModule(fragShdr, dev);

  // CREATE PIPELINE STAGE
  // VERTEX
  VkPipelineShaderStageCreateInfo vertShdrStageInfo;
  vertShdrStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShdrStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShdrStageInfo.module = vertShdrMod;
  vertShdrStageInfo.pName = "main";

  // CREATE PIPELINE STAGE
  // FRAGMENT
  VkPipelineShaderStageCreateInfo fragShdrStageInfo;
  fragShdrStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShdrStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShdrStageInfo.module = fragShdrMod;
  fragShdrStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShdrStageInfo,
                                                    fragShdrStageInfo};

  // DYNAMIC STATES CONFIG OF THE PIPELINE
  std::vector<VkDynamicState> dynStatesVec = {VK_DYNAMIC_STATE_VIEWPORT,
                                              VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynState{};
  dynState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynState.dynamicStateCount = static_cast<uint32_t>(dynStatesVec.size());
  dynState.pDynamicStates = dynStatesVec.data();

  // VERTEX BUFFER DATA
  VkPipelineVertexInputStateCreateInfo vertInptInfo{};
  vertInptInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertInptInfo.vertexBindingDescriptionCount = 0;
  vertInptInfo.pVertexBindingDescriptions = nullptr;
  vertInptInfo.vertexAttributeDescriptionCount = 0;
  vertInptInfo.pVertexAttributeDescriptions = nullptr;

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

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  // RASTERIZER
  VkPipelineRasterizationStateCreateInfo rstr{};
  rstr.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rstr.depthClampEnable = VK_FALSE;
  rstr.rasterizerDiscardEnable = VK_FALSE;
  rstr.polygonMode = VK_POLYGON_MODE_FILL;
  rstr.lineWidth = 1.0f;
  rstr.cullMode = VK_CULL_MODE_BACK_BIT;
  rstr.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rstr.depthBiasEnable = VK_FALSE;
  rstr.depthBiasConstantFactor = 0.0f;
  rstr.depthBiasClamp = 0.0f;
  rstr.depthBiasSlopeFactor = 0.0f;

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

  if (vkCreatePipelineLayout(dev, &pplnLytInfo, nullptr, &m_pplnLytVk) !=
      VK_SUCCESS) {
    std::runtime_error("FAILED TO CREATE PIPELINE LAYOUT!");
  };

  vkDestroyShaderModule(dev, vertShdrMod, nullptr);
  vkDestroyShaderModule(dev, fragShdrMod, nullptr);
};

void GfxPipelineVk::CleanUp(const VkDevice &dev) {
  vkDestroyPipelineLayout(dev, m_pplnLytVk, nullptr);
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

ENGINE_NAMESPACE_END
