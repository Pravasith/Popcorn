#pragma once

#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "RenderWorkflowVk.h"
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BasicRenderWorkflowVk : public RenderWorkflowVk {
public:
  BasicRenderWorkflowVk() {
    Init();
    PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
  };
  ~BasicRenderWorkflowVk() override {
    CleanUp();
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

private:
  virtual void Init() override {
    CreateRenderPasses();
    // CreateVkPipelines(const Material &);
  };
  virtual void CleanUp() override {
    // Cleanup render passes
    // Cleanup pipelines
  };

  virtual void CreateRenderPasses() override;
  virtual void CreateVkPipelines(const Material &) override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
