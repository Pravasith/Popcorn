#pragma once
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
enum class RendererType {
  OpenGL = 0,
  Vulkan = bit_shift_left(1),
  // DirectX = bit_shift_left(2),
  // Metal = bit_shift_left(3)
};

// SINGLETON
class Renderer {
public:
  static void Create();
  static void Destroy();
  Renderer &Get() const;

private:
  Renderer();
  virtual ~Renderer();

  static Renderer *s_instance;
  RendererType m_type;
};

ENGINE_NAMESPACE_END
