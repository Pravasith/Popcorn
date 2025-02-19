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
    PC_PRINT("CREATED", TagType::Constr, "BasicWorkflowVk")
  };
  ~BasicRenderWorkflowVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "BasicWorkflowVk")
  };

  virtual void CreateRenderPasses() override;
  virtual void CreateVkPipelines(const Material &) override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
