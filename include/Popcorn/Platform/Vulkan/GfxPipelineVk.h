#pragma once

#include "Global_Macros.h"
#include "ShaderVk.h"
#include <stdexcept>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class GfxPipelineVk {
  friend class RendererVk;

private:
  [[nodiscard]] inline const VkRenderPass &GetRndrPass() const {
    if (m_rndrPass == VK_NULL_HANDLE) {
      std::runtime_error("ERROR: RENDER PASS IS NULL");
    };

    return m_rndrPass;
  };

  [[nodiscard]] inline const VkPipeline &GetGfxPipeline() const {
    if (m_gfxPpline == VK_NULL_HANDLE) {
      std::runtime_error("ERROR: GFX PIPELINE IS NULL");
    };

    return m_gfxPpline;
  };

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
