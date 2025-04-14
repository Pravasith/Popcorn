#pragma once

#include "Camera.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "RenderFlows/RenderFlowVk.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GBufferRenderFlowVk : public RenderFlowVk {
public:
  GBufferRenderFlowVk()
      : m_pipelineFactory(PipelineFactoryVk::Get()),
        m_gBufferPipeline(
            m_pipelineFactory->GetGfxPipeline<Pipelines::Deferred>()) {
    PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
    //
    // --- Create frame resources -----------------------------------------
    CreateAttachments();
    CreateFramebuffer();
  };

  virtual ~GBufferRenderFlowVk() override {
    //
    // --- Cleanup frame resources ----------------------------------------
    CleanUp();
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

private:
  void CreateAttachments() override;
  void CreateFramebuffer() override;
  void CreateRenderPass() override;
  void CleanUp() override;

private:
  struct GBufferData {
    ImageVk albedoMap{};
    ImageVk depthMap{};
    ImageVk normalMap{};
  };

  GBufferData m_gBufferData{};

  // Renderpass & 2 subpasses
  RenderPassVk m_renderPass;
  VkFramebuffer m_framebuffer;

  VkSubpassDescription m_subpasses[2]{};

  Camera *camera;
  PipelineFactoryVk *m_pipelineFactory = nullptr;

  // Pipelines
  GBufferPipelineVk &m_gBufferPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
