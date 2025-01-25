#pragma once

#include "GlobalMacros.h"
#include "ShaderVk.h"
#include <stdexcept>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
class GfxPipelineVk {
  friend class RendererVk;

private:
  [[nodiscard]] inline const VkRenderPass &GetRenderPass() const {
    if (m_renderPass == VK_NULL_HANDLE) {
      std::runtime_error("ERROR: RENDER PASS IS NULL");
    };

    return m_renderPass;
  };

  [[nodiscard]] inline const VkPipeline &GetGfxPipeline() const {
    if (m_gfxPipeline == VK_NULL_HANDLE) {
      std::runtime_error("ERROR: GFX PIPELINE IS NULL");
    };

    return m_gfxPipeline;
  };

private:
  GfxPipelineVk() { PC_PRINT("CREATED", TagType::Constr, "GFX-PIPELINE-VK"); };
  ~GfxPipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "GFX-PIPELINE-VK");
  };

  void CreateGfxPipeline(const VkDevice &, const VkExtent2D &);
  VkShaderModule CreateShaderModule(const std::vector<char> &,
                                    const VkDevice &);
  void CreateRenderPass(const VkFormat &, const VkDevice &);
  void CleanUp(const VkDevice &);

private:
  ShaderVk m_ShaderVk;
  VkRenderPass m_renderPass;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_gfxPipeline;
};

ENGINE_NAMESPACE_END
