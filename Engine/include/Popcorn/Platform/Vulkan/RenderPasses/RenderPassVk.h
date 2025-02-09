#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class RenderPassVk {
  RenderPassVk() { PC_PRINT("CREATED", TagType::Constr, "RenderPassVk"); };
  ~RenderPassVk() { PC_PRINT("DESTROYED", TagType::Destr, "RenderPassVk"); };

  void CreateAttachment();
  void CreateSubpass();
  // void SpecifyAttachment
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
