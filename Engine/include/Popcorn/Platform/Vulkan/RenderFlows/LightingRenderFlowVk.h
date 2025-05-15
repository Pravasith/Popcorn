#pragma once

#include "AttachmentVk.h"
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

  virtual void CreateAndAllocDescriptors() override;
  virtual void CreatePipelines() override;
  virtual void DestroyPipelines() override;

  virtual void Paint(const uint32_t frameIndex, const uint32_t currentFrame,
                     VkCommandBuffer &currentFrameCommandBuffer) override;

private:
  struct AttachmentsVk {
    AttachmentVk lightAttachment{};
  };

  struct DescriptorSetsVk {
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> lightingSets{};
  };

  PcRenderFlowImages<RenderFlows::Lighting> &m_imagesVk = s_lightingImages;
  PcRenderFlowImages<RenderFlows::GBuffer> &m_dependencyImages =
      s_gBufferImages;

  AttachmentsVk m_attachmentsVk{};
  DescriptorSetsVk m_descriptorSetsVk{};

  RenderPassVk m_renderPass;
  VkFramebuffer m_framebuffer;

  // Pipelines
  LightingPipelineVk *m_lightingPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
