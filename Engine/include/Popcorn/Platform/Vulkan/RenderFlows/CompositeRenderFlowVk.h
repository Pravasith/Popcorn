#pragma once

#include "ContextVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "PipelineFactoryVk.h"
#include "RenderFlows/RenderFlowVk.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class CompositeRenderFlowVk : public RenderFlowVk {
public:
  CompositeRenderFlowVk() {
    PC_PRINT("CREATED", TagType::Constr, "CompositeRenderFlowVk")
  };

  virtual ~CompositeRenderFlowVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "CompositeRenderflowVk")
  };

  virtual void Prepare() override {
    m_compositePipeline =
        &ContextVk::PipelineFactory()->GetGfxPipeline<Pipelines::Composite>();

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
    ImageVk presentImage{};
  };

  Attachments m_attachments{};

  RenderPassVk m_renderPass;
  VkFramebuffer m_framebuffer;

  // Pipelines
  CompositePipelineVk *m_compositePipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
