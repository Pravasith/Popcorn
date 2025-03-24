#pragma once

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
class Material;

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

  // PASS IN SCENE & CAMERA ETC.
  virtual void ProcessScenes() = 0;
  virtual void CreateResources() = 0;
  virtual void DrawFrame(const Scene &scene) = 0;
  virtual bool OnFrameBufferResize(FrameBfrResizeEvent &) = 0;
  virtual void AddMeshToWorkflow(Mesh *meshPtr) = 0;

  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  Renderer(Renderer &&) = delete;
  Renderer &operator=(const Renderer &&) = delete;

protected:
  Renderer(const Window &);
  virtual ~Renderer();

  SceneLibrary m_sceneLibrary;

private:
  static RendererType s_type;
  static Renderer *s_instance;

protected:
  const Window &m_AppWin;

private:
  static void Init(const Window &);
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
