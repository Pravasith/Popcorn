#pragma once

#include "GlobalMacros.h"
#include "Renderer.h"
#include "Scene.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
class RendererOpenGL : public Renderer {
public:
  RendererOpenGL(const Window &appWin);
  virtual ~RendererOpenGL() override;

  virtual void ProcessScenes() override {};
  virtual void CreateResources() override {};
  virtual void DrawFrame(const Scene &) override {};
  virtual bool OnFrameBufferResize(FrameBfrResizeEvent &) override {
    return true;
  };
  virtual void AddMeshToWorkflow(Mesh *meshPtr) override {};
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
