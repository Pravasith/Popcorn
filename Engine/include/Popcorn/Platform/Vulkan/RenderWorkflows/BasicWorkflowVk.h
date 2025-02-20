#pragma once

#include "DeviceVk.h"
#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"
#include "RenderWorkflowVk.h"
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BasicRenderWorkflowVk : public RenderWorkflowVk {
public:
  BasicRenderWorkflowVk() {
    PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
  };
  ~BasicRenderWorkflowVk() override {
    CleanUp();
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

  virtual void CreateRenderPass() override;
  virtual void CreateVkPipeline(const Material &) override;

private:
  virtual void CleanUp() override {
    auto *deviceSingleton = DeviceVk::Get();
    // Cleanup pipelines
    m_basicGfxPipeline.Destroy();

    // Cleanup render passes
    m_basicRenderPass.Destroy(deviceSingleton->GetDevice());
  };

private:
  RenderPassVk m_basicRenderPass;
  GfxPipelineVk m_basicGfxPipeline;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
