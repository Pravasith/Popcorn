#pragma once

#include "Global_Macros.h"
#include "ShaderVk.h"
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class GfxPipelineVk {
  friend class RendererVk;

private:
  GfxPipelineVk() { PC_PRINT("CREATED", TagType::Constr, "GFX-PIPELINE-VK"); };
  ~GfxPipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "GFX-PIPELINE-VK");
  };

  void CreateGfxPipeline(const VkDevice &, const VkExtent2D &);
  VkShaderModule CreateShaderModule(const std::vector<char> &,
                                    const VkDevice &);
  void CreateRndrPass(const VkFormat &, const VkDevice &);
  void CleanUp(const VkDevice &);

private:
  ShaderVk m_ShdrVk;
  VkRenderPass m_rndrPass;
  VkPipelineLayout m_pplnLytVk;
  VkPipeline m_gfxPpline;
};

ENGINE_NAMESPACE_END
