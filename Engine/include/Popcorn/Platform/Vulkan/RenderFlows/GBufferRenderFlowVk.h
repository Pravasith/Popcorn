#pragma once

#include "AttachmentVk.h"
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

private:
  virtual void CreateAttachments() override;
  virtual void CreateFramebuffer() override;
  virtual void CreateRenderPass() override;

  virtual void DestroyRenderPass() override;
  virtual void DestroyFramebuffer() override;
  virtual void DestroyAttachments() override;

  virtual void CreateAndAllocDescriptors() override;
  virtual void CreatePipelines() override;

private:
  struct ImagesVk {
    ImageVk albedoImage{};
    ImageVk depthImage{};
    ImageVk normalImage{};
  };

  struct AttachmentsVk {
    AttachmentVk albedoAttachment{};
    AttachmentVk depthAttachment{};
    AttachmentVk normalAttachment{};
  };

  ImagesVk m_imagesVk{};
  AttachmentsVk m_attachmentsVk{};

  RenderPassVk m_renderPass;
  VkFramebuffer m_framebuffer;

  GBufferPipelineVk *m_gBufferPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
