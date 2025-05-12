#pragma once

#include "AttachmentVk.h"
#include "CommonVk.h"
#include "GBufferPipelineVk.h"
#include "GlobalMacros.h"
#include "RenderFlows/RenderFlowVk.h"
#include "RenderPassVk.h"
#include <array>
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
  struct AttachmentsVk {
    AttachmentVk albedoAttachment{};
    AttachmentVk depthAttachment{};
    AttachmentVk normalAttachment{};
  };

  struct DescriptorSetsVk {
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> cameraSets{};
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> submeshSets{};
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> basicMatSets{};
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> pbrMatSets{};
  };

  PcRenderFlowImages<RenderFlows::GBuffer> &m_imagesVk = s_gBufferImages;

  AttachmentsVk m_attachmentsVk{};
  DescriptorSetsVk m_descriptorSetsVk{};

  RenderPassVk m_renderPass;
  VkFramebuffer m_framebuffer;

  GBufferPipelineVk *m_gBufferPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
