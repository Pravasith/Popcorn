#pragma once

#include "GlobalMacros.h"
#include "Renderer.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
class RendererOpenGL : public Renderer {
public:
  RendererOpenGL(const Window &appWin);
  ~RendererOpenGL();
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
