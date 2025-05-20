#pragma once

#include "GlobalMacros.h"
#include "PipelineVk.h"
#include "Popcorn/Core/Base.h"
#include <vulkan/vulkan_core.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// --- BASIC MATERIAL PIPELINE -----------------------------------------------
class BasicMatPipelineVk : public PipelineVk<PipelineTypes::GraphicsType> {
public:
  BasicMatPipelineVk() {
    PC_PRINT("CREATED", TagType::Constr, "BasicMatPipelineVk");
  };
  ~BasicMatPipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "BasicMatPipelineVk");
  };

  virtual void Create(const BufferDefs::Layout &vertexBufferLayout) override;
};

//
// --- PBR MATERIAL PIPELINE -------------------------------------------------
class PbrMatPipelineVk : public PipelineVk<PipelineTypes::GraphicsType> {
public:
  PbrMatPipelineVk() {
    PC_PRINT("CREATED", TagType::Constr, "PbrMatPipelineVk");
  };
  ~PbrMatPipelineVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "PbrMatPipelineVk");
  };

  virtual void Create(const BufferDefs::Layout &vertexBufferLayout) override;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
