#pragma once

#include "GlobalMacros.h"
#include "Renderer.h"

ENGINE_NAMESPACE_BEGIN
class RendererOpenGL : public Renderer {
public:
  RendererOpenGL(const Window &appWin);
  ~RendererOpenGL();
};

ENGINE_NAMESPACE_END
