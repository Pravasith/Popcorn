#pragma once

#include "CommonVk.h"
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
  LightingRenderFlowVk()
      : m_imagesVk(s_lightingImages), m_dependencyImages(s_gBufferImages) {
    m_isFirstTimeInFrame.fill(true);
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
    PcFramesAttachments<MAX_FRAMES_IN_FLIGHT> lightAttachments{};
  };

  struct DescriptorSetsVk {
    PcFramesDescriptorSets<MAX_FRAMES_IN_FLIGHT> lightingSets{};
  };

  struct ImageBarriersVk {
    PcFramesColorImageBarriers lightBarriers;
  };

  PcRenderFlowImages<RenderFlows::Lighting, MAX_FRAMES_IN_FLIGHT> &m_imagesVk;
  PcRenderFlowImages<RenderFlows::GBuffer, MAX_FRAMES_IN_FLIGHT>
      &m_dependencyImages;

  AttachmentsVk m_attachmentsVk{};
  DescriptorSetsVk m_descriptorSetsVk{};

  RenderPassVk m_renderPass;
  PcFramesFramebuffers<MAX_FRAMES_IN_FLIGHT> m_framebuffers;

  // Pipelines
  LightingPipelineVk m_lightingPipelineVk;

  std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> m_commandBuffers{};
  std::array<bool, MAX_FRAMES_IN_FLIGHT> m_isFirstTimeInFrame{};

  ImageBarriersVk m_imageBarriers;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
