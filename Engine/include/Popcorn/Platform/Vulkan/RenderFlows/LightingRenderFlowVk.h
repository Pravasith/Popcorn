#pragma once

#include "ContextVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "PipelineFactoryVk.h"
#include "RenderFlows/RenderFlowVk.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class LightingRenderFlowVk : public RenderFlowVk {
public:
  LightingRenderFlowVk() {
    PC_PRINT("CREATED", TagType::Constr, "LightingRenderFlowVk")
  };

  virtual ~LightingRenderFlowVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "LightingRenderflowVk")
  };

  virtual void Prepare() override {
    m_lightingPipeline =
        &ContextVk::PipelineFactory()->GetGfxPipeline<Pipelines::Lighting>();

    CreateAttachments();
    CreateRenderPass();
    CreateFramebuffer();
  };
  virtual void CleanUp() override;

private:
  void CreateAttachments() override;
  void CreateFramebuffer() override;
  void CreateRenderPass() override;

private:
  struct LightingData {
    ImageVk albedoMap{};
    ImageVk depthMap{};
    ImageVk normalMap{};
  };

  LightingData m_lightingData{};

  RenderPassVk m_renderPass;
  VkFramebuffer m_framebuffer;

  // Pipelines
  LightingPipelineVk *m_lightingPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
