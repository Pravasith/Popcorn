#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Window.h"
#include "Popcorn/Events/WindowEvent.h"
#include <variant>

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
  static Renderer &Get() { return *GetRenderer(); };
  static Renderer *GetRenderer();

  [[nodiscard]] const static RendererType GetAPI() { return s_type; };

  // PASS IN SCENE & CAMERA ETC.
  virtual void DrawFrame();
  virtual bool OnFrameBfrResize(FrameBfrResizeEvent &);

  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  Renderer(Renderer &&) = delete;
  Renderer &operator=(const Renderer &&) = delete;

  Renderer(const Window &);
  virtual ~Renderer();

private:
  Renderer();

  static RendererType s_type;
  static Renderer *s_instance;
  static std::variant<RendererVk *, RendererOpenGL *> s_renderer;

  static VertexBuffer *s_vertexBuffer;

  void Init() const;

protected:
  const Window &m_AppWin;
};

template <RendererType T> Renderer *Renderer::Create(const Window &appWin) {
  if (!s_instance) {
    // PC_ASSERT(s_instance, "NO RENDERER INSTANCE");
    s_type = T;
    s_instance = new Renderer(appWin);
    s_instance->Init();
  }

  return s_instance;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
