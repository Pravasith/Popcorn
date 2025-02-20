#pragma once

#include "GlobalMacros.h"
#include "Renderer.h"
#include "Scene.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
class RendererOpenGL : public Renderer {
public:
  RendererOpenGL(const Window &appWin);
  ~RendererOpenGL() override;

  virtual void DrawFrame(const Scene &) const override {};
  virtual bool OnFrameBfrResize(FrameBfrResizeEvent &) override {
    return false;
  };

  virtual void PrepareMaterialForRender(Material *materialPtr) override {};
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
