#pragma once

#include "GfxContext.h"
#include "GlobalMacros.h"
#include "Mesh.h"
#include "Popcorn/Core/Window.h"
#include "Popcorn/Events/WindowEvent.h"
#include "SceneLibrary.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class RendererType {
  None = 0,
  OpenGL = shift_l(1),
  Vulkan = shift_l(2),
  // DirectX = shift_l(3),
  // Metal = shift_l(4)
};

class RendererOpenGL;
class RendererVk;
class VertexBuffer;
class Scene;

// SINGLETON
class Renderer {
public:
  template <RendererType T> static Renderer *Create(const Window &);
  static void Destroy();
  static Renderer &Get() { return *s_instance; };

  [[nodiscard]] const static RendererType GetAPI() {
    if (s_type == RendererType::None) {
      PC_WARN("Trying to access an undefined s_type!")
    };

    return s_type;
  };

  void AddScene(Scene *scene) { m_sceneLibrary.Add(scene); };
  void RemoveScene(Scene *scene) { m_sceneLibrary.Remove(scene); };

  virtual void CreateRenderFlows() = 0;
  virtual void AssignSceneObjectsToRenderFlows() = 0;
  virtual void CreateRenderFlowResources() = 0;

  virtual void DrawFrame(const Scene &scene) = 0;

  // Utils
  virtual bool OnFrameBufferResize(FrameBfrResizeEvent &) = 0;

  // DELETE COPY & MOVE OPERATIONS
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;
  Renderer(Renderer &&) = delete;
  Renderer &operator=(const Renderer &&) = delete;

protected:
  Renderer(const Window &);
  virtual ~Renderer();

private:
  static void Init(const Window &);

protected:
  SceneLibrary m_sceneLibrary;
  const Window &m_AppWin;

private:
  static RendererType s_type;
  static Renderer *s_instance;
  static GfxContext *s_gfxContext;
};

template <RendererType T> Renderer *Renderer::Create(const Window &appWin) {
  if (!s_instance) {
    // PC_ASSERT(s_instance, "NO RENDERER INSTANCE");
    s_type = T;

    // s_instance = new Renderer(appWin);
    Renderer::Init(appWin);
  }

  return s_instance;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
