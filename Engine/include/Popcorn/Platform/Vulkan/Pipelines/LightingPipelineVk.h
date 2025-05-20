#pragma once

#include "GlobalMacros.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class LightingPipelineVk : public PipelineVk<PipelineTypes::GraphicsType> {
public:
  LightingPipelineVk() {
    PC_PRINT("CREATED", TagType::Constr, "LightingPipelineVk");
  };
  ~LightingPipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "LightingPipelineVk");
  };

  virtual void Create(const BufferDefs::Layout &vertexBufferLayout) override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
