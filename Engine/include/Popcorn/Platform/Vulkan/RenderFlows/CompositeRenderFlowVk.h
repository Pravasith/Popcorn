#pragma once

#include "CompositePipelinesVk.h"
#include "GlobalMacros.h"
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
  }

  virtual ~CompositeRenderFlowVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "CompositeRenderflowVk")
  }

private:
  virtual void CreateAttachments() override;
  virtual void CreateImageBarriers() override;
  virtual void CreateRenderPass() override;
  virtual void CreateFramebuffers() override;

  virtual void DestroyFramebuffers() override;
  virtual void DestroyRenderPass() override;
  virtual void DestroyAttachments() override;

  virtual void AllocDescriptorsLocal() override;
  virtual void UpdateDescriptorSetsLocal() override;

  virtual void CreatePipelines() override;
  virtual void DestroyPipelines() override;

  virtual void CreateCommandBuffers() override;

  [[nodiscard]] virtual std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> &
  GetCommandBuffers() override {
    return m_commandBuffers;
  };

  virtual void OnSwapchainInvalidCb() override;
  virtual void RecordCommandBuffer(const uint32_t frameIndex,
                                   const uint32_t currentFrame) override;

private:
  struct AttachmentsVk {
    PcPresentationAttachments presentAttachments{};
  };

  struct DescriptorSetsVk {
    PcFramesDescriptorSets_Bundle<MAX_FRAMES_IN_FLIGHT> presentSets{};
  };

  struct ImageBarriersVk {
    PcPresentationImageBarriers presentBarriers;
  };

  AttachmentsVk m_attachmentsVk{};
  DescriptorSetsVk m_descriptorSetsVk{};

  RenderPassVk m_renderPass;
  PcPresentationFramebuffers m_framebuffers; // Swapchain framebuffers

  // Pipelines
  CompositePipelineVk m_compositePipelineVk;

  std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> m_commandBuffers{};

  ImageBarriersVk m_imageBarriers;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
