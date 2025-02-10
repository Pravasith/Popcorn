#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Window.h"
#include "Popcorn/Events/WindowEvent.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class RendererType {
  OpenGL = shift_l(0),
  Vulkan = shift_l(1),
  // DirectX = shift_l(2),
  // Metal = shift_l(3)
};

class RendererOpenGL;
class RendererVk;
class VertexBuffer;

// SINGLETON
class Renderer {

public:
  template <RendererType T> static Renderer *Create(const Window &);
  static void Destroy();

  // template <RendererType T>
  static Renderer &Get() { return *s_instance; };

  [[nodiscard]] const static RendererType GetAPI() { return s_type; };

  // PASS IN SCENE & CAMERA ETC.
  virtual void DrawFrame() = 0;
  virtual void PresentFrame() = 0;
  virtual bool OnFrameBfrResize(FrameBfrResizeEvent &) = 0;

  // TODO: Abstract everything in here
  void CreateTerrainMesh() {};

  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  Renderer(Renderer &&) = delete;
  Renderer &operator=(const Renderer &&) = delete;

  Renderer(const Window &);

  Renderer();
  virtual ~Renderer();

private:
  static RendererType s_type;
  static Renderer *s_instance;
  static VertexBuffer *s_vertexBuffer;

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
