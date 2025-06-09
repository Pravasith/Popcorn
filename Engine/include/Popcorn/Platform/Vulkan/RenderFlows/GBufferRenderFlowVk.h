#pragma once

#include "CommonVk.h"
#include "GlobalMacros.h"
#include "MaterialPipelinesVk.h"
#include "RenderFlows/RenderFlowDefs.h"
#include "RenderFlows/RenderFlowVk.h"
#include "RenderPassVk.h"
#include <array>
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GBufferRenderFlowVk : public RenderFlowVk {
public:
  GBufferRenderFlowVk()
      : m_imagesVk(s_gBufferImages) {
          PC_PRINT("CREATED", TagType::Constr, "GBufferRenderFlowVk")
        };

  virtual ~GBufferRenderFlowVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "GBufferRenderflowVk")
  };

private:
  virtual void CreateAttachments() override;
  virtual void CreateImageBarriers() override;
  virtual void CreateFramebuffers() override;
  virtual void CreateRenderPass() override;

  virtual void DestroyRenderPass() override;
  virtual void DestroyFramebuffers() override;
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
    PcFramesAttachments<MAX_FRAMES_IN_FLIGHT> albedoAttachments{};
    PcFramesAttachments<MAX_FRAMES_IN_FLIGHT> depthAttachments{};
    PcFramesAttachments<MAX_FRAMES_IN_FLIGHT> normalAttachments{};
    PcFramesAttachments<MAX_FRAMES_IN_FLIGHT> roughnessMetallicAttachments{};
  };

  struct DescriptorSetsVk {
    PcFramesDescriptorSets<MAX_FRAMES_IN_FLIGHT> submeshSets{};
    PcFramesDescriptorSets<MAX_FRAMES_IN_FLIGHT> basicMatSets{};
    PcFramesDescriptorSets<MAX_FRAMES_IN_FLIGHT> pbrMatSets{};
  };

  struct ImageBarriersVk {
    PcFramesColorImageBarriers albedoBarriers;
    PcFramesDepthImageBarriers depthBarriers;
    PcFramesColorImageBarriers normalBarriers;
    PcFramesColorImageBarriers roughnessMetallicBarriers;
  };

  PcRenderFlowImages<RenderFlows::GBuffer, MAX_FRAMES_IN_FLIGHT> &m_imagesVk;

  AttachmentsVk m_attachmentsVk{};
  DescriptorSetsVk m_descriptorSetsVk{};

  RenderPassVk m_renderPass;
  PcFramesFramebuffers<MAX_FRAMES_IN_FLIGHT> m_framebuffers;

  BasicMatPipelineVk m_basicMatPipelineVk;
  PbrMatPipelineVk m_pbrMatPipelineVk;

  std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> m_commandBuffers{};

  ImageBarriersVk m_imageBarriers;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
