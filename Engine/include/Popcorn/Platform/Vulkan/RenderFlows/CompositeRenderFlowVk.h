#pragma once

#include "AttachmentVk.h"
#include "CompositePipelineVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "RenderFlows/RenderFlowVk.h"
#include "RenderPassVk.h"
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
  struct AttachmentsVk {
    AttachmentVk presentAttachment{};
  };

  struct DescriptorSetsVk {
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> presentSets{};
  };

  PcRenderFlowImages<RenderFlows::Composite> &m_imagesVk = s_compositeImages;
  PcRenderFlowImages<RenderFlows::Lighting> &m_dependencyImages =
      s_lightingImages;

  AttachmentsVk m_attachmentsVk{};
  DescriptorSetsVk m_descriptorSetsVk{};

  RenderPassVk m_renderPass;
  VkFramebuffer m_framebuffer;

  // Pipelines
  CompositePipelineVk *m_compositePipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
