#pragma once

#include "GlobalMacros.h"
#include "ShaderVk.h"
#include "VertexBufferVk.h"
#include <stdexcept>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

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

  inline void AttachVertexBuffer(const VertexBufferVk &vBfr) {
    PC_PRINT("VERTEX BUFFER", TagType::Print, "ELEMENT OFFSETS")
    m_vertexBufferVk = std::move(vBfr);

    auto *bfr = &m_vertexBufferVk;

    for (auto elType : bfr->GetLayout().attrTypesValue) {
      PC_PRINT(static_cast<int>(elType), TagType::Print, "ELEMENT TYPES")
    }
    for (auto elType : bfr->GetLayout().attrOffsetsValue) {
      PC_PRINT(elType, TagType::Print, "ELEMENT OFFSETS")
    }
    PC_PRINT(bfr->GetLayout().attrOffsetsValue.size(), TagType::Print,
             "OFFSET SIZE")
    PC_PRINT(bfr->GetLayout().strideValue, TagType::Print, "LAYOUT STRIDE")
    PC_PRINT(bfr->GetLayout().countValue, TagType::Print, "LAYOUT COUNT")
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
  VertexBufferVk m_vertexBufferVk;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
