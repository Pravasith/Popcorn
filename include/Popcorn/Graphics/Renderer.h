#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
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
  static Renderer *Create();
  static void Destroy();

  static void SetOSWindow(void *);

  void Init() const;

  static const auto GetRenderer();

  virtual void OnUpdate();

  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  Renderer(Renderer &&) = delete;
  Renderer &operator=(const Renderer &&) = delete;

  Renderer();
  virtual ~Renderer();

private:
  constexpr static RendererType s_type = T;
  static Renderer *s_instance;
  static std::variant<RendererVk *, RendererOpenGL *> s_renderer;

protected:
  static void *s_osWindow;
};

ENGINE_NAMESPACE_END
