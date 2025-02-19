#pragma once

#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"
#include <vector>
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

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

private:
  virtual void Init() = 0;
  virtual void CleanUp() = 0;

  virtual void CreateRenderPasses() = 0;
  virtual void CreateVkPipelines(const Material &) = 0;

protected:
  std::vector<RenderPassVk> m_renderPasses;
  std::vector<GfxPipelineVk> m_gfxPipelines;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
