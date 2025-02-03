#pragma once

#include "GlobalMacros.h"
#include "Renderer.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
class RendererOpenGL : public Renderer {
public:
  RendererOpenGL(const Window &appWin);
  ~RendererOpenGL() override;

  virtual void DrawFrame() override {};
  virtual void PresentFrame() override {};
  virtual bool OnFrameBfrResize(FrameBfrResizeEvent &) override {
    return false;
  };
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
