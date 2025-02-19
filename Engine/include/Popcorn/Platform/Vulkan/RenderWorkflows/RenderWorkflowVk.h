#pragma once

#include "GfxPipelineVk.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "RenderPassVk.h"
#include <vector>
#include <vulkan/vulkan_core.h>

// TODO: Redo this class. But only after a full working animated scene is
// rendered on screen
ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class RenderWorkflowVk {
public:
  RenderWorkflowVk() {
    PC_PRINT("CREATED", TagType::Constr, "RenderWorkflowVk")
  };
  virtual ~RenderWorkflowVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "RenderWorkflowVk")
  };

  virtual void CreateRenderPasses() = 0;
  virtual void CreateVkPipelines(const Material &) = 0;

protected:
  std::vector<RenderPassVk> m_renderPasses;
  std::vector<GfxPipelineVk> m_gfxPipelines;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
