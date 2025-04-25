#pragma once

#include "GlobalMacros.h"
#include "ImageVk.h"
#include "LightingPipelineVk.h"
#include "RenderFlows/RenderFlowVk.h"
#include "RenderPassVk.h"
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

private:
  virtual void CreateAttachments() override;
  virtual void CreateRenderPass() override;
  virtual void CreateFramebuffer() override;

  virtual void DestroyFramebuffer() override;
  virtual void DestroyRenderPass() override;
  virtual void DestroyAttachments() override;

  virtual void CreatePipelines() override;
  virtual void CreateAndAllocDescriptors() override;

private:
  struct Attachments {
    ImageVk lightBuffer{};
  };

  Attachments m_attachments{};

  RenderPassVk m_renderPass;
  VkFramebuffer m_framebuffer;

  // Pipelines
  LightingPipelineVk *m_lightingPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
