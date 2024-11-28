#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <variant>

ENGINE_NAMESPACE_BEGIN

enum class RendererType {
  None = 0,
  OpenGL = shift_l(1),
  Vulkan = shift_l(2),
  // DirectX = shift_l(3),
  // Metal = shift_l(4)
};

// FORWARD DECLARATIONS OF DERIVED CLASSES TO AVOID
// CIRCULAR DEP PROBLEM
class RendererOpenGL;
class RendererVk;

// SINGLETON
class Renderer {
public:
  static const Renderer *Create();
  static void Destroy();

  static void SetOSWindow(void *);

  void Init() const;

  static Renderer &Get();

  virtual void OnUpdate() const {};

  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  Renderer(Renderer &&) = delete;
  Renderer &operator=(const Renderer &&) = delete;

protected:
  Renderer();
  virtual ~Renderer();

  static Renderer *s_instance;
  static RendererType s_type;
  static void *s_osWindow;

  static std::variant<RendererVk *, RendererOpenGL *> s_renderer;
};

ENGINE_NAMESPACE_END
