#pragma once

#include "ContextVk.h"
#include "GBufferPipelineVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "PipelineFactoryVk.h"
#include "RenderFlows/RenderFlowVk.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GBufferRenderFlowVk : public RenderFlowVk {
public:
  GBufferRenderFlowVk() {
    PC_PRINT("CREATED", TagType::Constr, "GBufferRenderFlowVk")
  };

  virtual ~GBufferRenderFlowVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "GBufferRenderflowVk")
  };

  virtual void Prepare() override {
    m_gBufferPipeline =
        &ContextVk::PipelineFactory()->GetGfxPipeline<Pipelines::Deferred>();

    CreateAttachments();
    CreateRenderPass();
    CreateFramebuffer();
  };
  virtual void CleanUp() override;

private:
  void CreateAttachments() override;
  void CreateFramebuffer() override;
  void CreateRenderPass() override;
  void CreatePipelines() override;

private:
  struct Attachments {
    ImageVk albedoImage{};
    ImageVk depthImage{};
    ImageVk normalImage{};
  };

  Attachments m_attachments{};

  RenderPassVk m_renderPass;
  VkFramebuffer m_framebuffer;

  GBufferPipelineVk *m_gBufferPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
