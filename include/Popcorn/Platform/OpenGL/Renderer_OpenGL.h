#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
ENGINE_NAMESPACE_BEGIN
class RendererOpenGL {
public:
  RendererOpenGL() { PC_PRINT_DEBUG("OPENGL CREATED", 2, "RENDERER") };
  ~RendererOpenGL() { PC_PRINT_DEBUG("OPENGL DESTROYED", 2, "RENDERER") };
};

ENGINE_NAMESPACE_END
