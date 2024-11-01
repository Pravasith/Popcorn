#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Window.h"
#include "Popcorn/Events/Subscriber.h"

ENGINE_NAMESPACE_BEGIN
enum class RendererType {
  None = 0,
  OpenGL = bit_shift_left(1),
  Vulkan = bit_shift_left(2),
  // DirectX = bit_shift_left(3),
  // Metal = bit_shift_left(4)
};

// SINGLETON
class Renderer : public Subscriber {
public:
  static void Create();
  static void Destroy();
  Renderer &Get() const;
  void OnEvent(Event &) const override {};
  // Window &GetWindow() const {};

private:
  Renderer();
  virtual ~Renderer();

  static Renderer *s_instance;
  RendererType m_type;
};

ENGINE_NAMESPACE_END
