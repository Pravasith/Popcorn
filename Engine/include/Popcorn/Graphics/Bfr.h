#pragma once

#include "GlobalMacros.h"
#include <glm/glm.hpp>
#include <initializer_list>
#include <tuple>
#include <vector>

ENGINE_NAMESPACE_BEGIN
enum class ElementType {
  None = 0,
  Float,
  Float2,
  Float3,
  Float4,
  Mat3,
  Mat4,
  Int,
  Int2,
  Int3,
  Int4,
  Bool
};

template <ElementType... Args> class Buffer {
public:
  Buffer() = default;
  virtual ~Buffer() = default;
};

// Buffer<ElementType::Float2, ElementType::Float3> bfr = {
//     {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
//     {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

ENGINE_NAMESPACE_END
