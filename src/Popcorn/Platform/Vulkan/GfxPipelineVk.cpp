#include "GfxPipelineVk.h"
#include "Global_Macros.h"
#include "Sources.h"
#include <stdexcept>
#include <vector>

ENGINE_NAMESPACE_BEGIN

void GfxPipelineVk::CreateGfxPipeline(const VkDevice &dev) {
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

  vkDestroyShaderModule(dev, vertShdrMod, nullptr);
  vkDestroyShaderModule(dev, fragShdrMod, nullptr);
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
