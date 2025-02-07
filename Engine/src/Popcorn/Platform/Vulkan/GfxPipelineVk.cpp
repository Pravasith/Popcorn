#include "Pipelines/GfxPipelineVk.h"
#include "GlobalMacros.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

std::vector<VkPipelineShaderStageCreateInfo> GfxPipelineVk::CreateShaderStages(
    std::forward_list<VkShaderModule> shaderModules) {

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
  if (m_enabledShaderStagesMask & ShaderStages::Tesselation) {
    // TODO: FILL THIS OUT
    // shaderStages.emplace_back(tessShaderStageInfo);
  };

  //
  // GEOMETRY STAGE ---------------------------------------------------
  if (m_enabledShaderStagesMask & ShaderStages::Geometry) {
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

void GfxPipelineVk::CreatePipeline() {};

void GfxPipelineVk::ConfigureDynamicStates() {};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
