#pragma once

#include "AttachmentVk.h"
#include "GlobalMacros.h"
#include "LightingPipelinesVk.h"
#include "RenderFlows/RenderFlowDefs.h"
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
    PcFramesAttachments lightAttachments{};
  };

  struct DescriptorSetsVk {
    PcFramesDescriptorSets lightingSets{};
  };

  PcRenderFlowImages<RenderFlows::Lighting> &m_imagesVk = s_lightingImages;
  PcRenderFlowImages<RenderFlows::GBuffer> &m_dependencyImages =
      s_gBufferImages;

  AttachmentsVk m_attachmentsVk{};
  DescriptorSetsVk m_descriptorSetsVk{};

  RenderPassVk m_renderPass;
  PcFramesFramebuffers m_framebuffers;

  // Pipelines
  LightingPipelineVk *m_lightingPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
