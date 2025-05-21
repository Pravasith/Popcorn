#pragma once

#include "AttachmentVk.h"
#include "CompositePipelineVk.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "RenderFlows/RenderFlowDefs.h"
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
  virtual void CreateFramebuffers() override;

  virtual void DestroyFramebuffers() override;
  virtual void DestroyRenderPass() override;
  virtual void DestroyAttachments() override;

  virtual void CreateAndAllocDescriptors() override;
  virtual void CreatePipelines() override;
  virtual void DestroyPipelines() override;

  virtual void Paint(const uint32_t frameIndex, const uint32_t currentFrame,
                     VkCommandBuffer &currentFrameCommandBuffer) override;

private:
  struct AttachmentsVk {
    PcFramesAttachments presentAttachments{};
  };

  struct DescriptorSetsVk {
    PcFramesDescriptorSets presentSets{};
  };

  PcRenderFlowImages<RenderFlows::Composite> &m_imagesVk = s_compositeImages;
  PcRenderFlowImages<RenderFlows::Lighting> &m_dependencyImages =
      s_lightingImages;

  AttachmentsVk m_attachmentsVk{};
  DescriptorSetsVk m_descriptorSetsVk{};

  RenderPassVk m_renderPass;
  PcFramesFramebuffers m_framebuffers;

  // Pipelines
  CompositePipelineVk *m_compositePipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
