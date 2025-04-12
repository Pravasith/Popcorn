#pragma once

#include "Camera.h"
#include "GlobalMacros.h"
#include "ImageVk.h"
#include "RenderFlows/RenderFlowVk.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GBufferRenderFlowVk : public RenderFlowVk {
public:
  void CreateAttachments() override;
  void CreateFrameBuffer();

  //
  // --- CONSTRUCTOR & DESTRUCTOR -------------------------------------------
  GBufferRenderFlowVk()
      : m_pipelineFactory(PipelineFactoryVk::Get()),
        m_gBufferPipeline(
            m_pipelineFactory->GetGfxPipeline<Pipelines::Deferred>()) {
          PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
        };

  virtual ~GBufferRenderFlowVk() override {
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

private:
  struct GBufferData {
    ImageVk albedoMap{};
    ImageVk depthMap{};
    ImageVk normalMap{};
  };

  GBufferData m_gBufferData{};

  // Renderpass & 2 subpasses
  RenderPassVk m_renderPass;
  VkSubpassDescription m_subpasses[2]{};

  Camera *camera;
  PipelineFactoryVk *m_pipelineFactory = nullptr;

  // Pipelines
  GBufferPipelineVk &m_gBufferPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
