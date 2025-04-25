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

  virtual void CreateRenderFlows() override {};
  virtual void PrepareRenderFlows() override {};
  virtual void AssignSceneObjectsToRenderFlows() override {};
  virtual void CreateRenderFlowResources() override {};

  virtual void ProcessGameObjectNode(GameObject *node) override {};

  virtual void DrawFrame(const Scene &) override {};

  virtual bool OnFrameBufferResize(FrameBfrResizeEvent &) override {
    return true;
  };
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
