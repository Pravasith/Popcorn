#pragma once

#include "GBufferPipelineVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "RenderFlows/RenderFlowVk.h"
#include "RenderPassVk.h"
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

  virtual void CleanUp() override;

private:
  virtual void CreateAttachments() override;
  virtual void CreateFramebuffer() override;
  virtual void CreateRenderPass() override;
  virtual void CreateAndAllocDescriptors() override;
  virtual void CreatePipelines() override;

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
