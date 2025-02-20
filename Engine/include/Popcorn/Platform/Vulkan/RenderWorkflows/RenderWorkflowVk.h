#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Material;

enum class RenderWorkflowIndices {
  Basic = 1, // Basic colors
  Shadows,   // Simple hard shadows
};

class RenderWorkflowVk {
public:
  RenderWorkflowVk() {
    PC_PRINT("CREATED", TagType::Constr, "RenderWorkflowVk")
  };
  virtual ~RenderWorkflowVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "RenderWorkflowVk")
  };

  virtual void CreateRenderPass() = 0;
  virtual void CreateVkPipeline(const Material &) = 0;

private:
  virtual void CleanUp() = 0;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
