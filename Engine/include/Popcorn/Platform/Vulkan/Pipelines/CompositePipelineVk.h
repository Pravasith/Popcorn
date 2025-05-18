#pragma once

#include "GlobalMacros.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class CompositePipelineVk : public PipelineVk<PipelineTypes::GraphicsType> {
public:
  CompositePipelineVk() {
    PC_PRINT("CREATED", TagType::Constr, "CompositePipelineVk");
  };
  ~CompositePipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "CompositePipelineVk");
  };

  virtual void Create() override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
