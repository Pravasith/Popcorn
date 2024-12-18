#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Window.h"
#include <variant>

ENGINE_NAMESPACE_BEGIN

enum class RendererType {
  OpenGL = shift_l(0),
  Vulkan = shift_l(1),
  // DirectX = shift_l(2),
  // Metal = shift_l(3)
};

// FORWARD DECLARATIONS OF DERIVED CLASSES TO AVOID
// CIRCULAR DEP PROBLEM
class RendererOpenGL;
class RendererVk;

// SINGLETON
template <RendererType T> class Renderer {
public:
  static Renderer *Create(const Window &appWin);
  static void Destroy();

  static const auto GetRenderer();

  virtual void OnUpdate();
  virtual void OnEvent(Event &);

  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  Renderer(Renderer &&) = delete;
  Renderer &operator=(const Renderer &&) = delete;

  Renderer(const Window &);
  virtual ~Renderer();

private:
  constexpr static RendererType s_type = T;
  static Renderer *s_instance;
  static std::variant<RendererVk *, RendererOpenGL *> s_renderer;

  void Init() const;

protected:
  const Window &m_AppWin;
};

ENGINE_NAMESPACE_END
