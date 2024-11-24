#pragma once

#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
class GfxPipelineVk {
private:
  friend class RendererVk;

  void CreateGfxPipeline();
};

ENGINE_NAMESPACE_END
