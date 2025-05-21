#pragma once

#include "GlobalMacros.h"
#include "MaterialPipelinesVk.h"
#include "RenderFlows/RenderFlowDefs.h"
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
  virtual void CreateFramebuffers() override;
  virtual void CreateRenderPass() override;

  virtual void DestroyRenderPass() override;
  virtual void DestroyFramebuffers() override;
  virtual void DestroyAttachments() override;

  virtual void CreateAndAllocDescriptors() override;
  virtual void CreatePipelines() override;
  virtual void DestroyPipelines() override;

  virtual void Paint(const uint32_t frameIndex, const uint32_t currentFrame,
                     VkCommandBuffer &currentFrameCommandBuffer) override;

private:
  struct AttachmentsVk {
    PcFramesAttachments albedoAttachments{};
    PcFramesAttachments depthAttachments{};
    PcFramesAttachments normalAttachments{};
    PcFramesAttachments roughnessMetallicAttachments{};
  };

  struct DescriptorSetsVk {
    PcFramesDescriptorSets cameraSets{};
    PcFramesDescriptorSets submeshSets{};
    PcFramesDescriptorSets basicMatSets{};
    PcFramesDescriptorSets pbrMatSets{};
  };

  PcRenderFlowImages<RenderFlows::GBuffer> &m_imagesVk = s_gBufferImages;

  AttachmentsVk m_attachmentsVk{};
  DescriptorSetsVk m_descriptorSetsVk{};

  RenderPassVk m_renderPass;
  PcFramesFramebuffers m_framebuffers;

  BasicMatPipelineVk *m_basicMatPipeline = nullptr;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
