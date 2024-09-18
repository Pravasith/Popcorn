#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
ENGINE_NAMESPACE_BEGIN
class RendererVulkan {
public:
  RendererVulkan() { PC_PRINT_DEBUG("VULKAN CREATED", 2, "RENDERER") };
  ~RendererVulkan() { PC_PRINT_DEBUG("VULKAN DESTROYED", 2, "RENDERER") };
};

ENGINE_NAMESPACE_END
