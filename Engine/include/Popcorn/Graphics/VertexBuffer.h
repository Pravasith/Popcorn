#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cassert>
#include <cstring>
#include <initializer_list>

ENGINE_NAMESPACE_BEGIN
namespace Gfx {

template <typename T> class Buffer;

// Enum for ElementType
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

template <typename T> class VertexBuffer {
  VertexBuffer(std::initializer_list<T>);
  // { PC_PRINT("CREATED", TagType::Constr, "VERTEX-BUFFER") };
  virtual ~VertexBuffer() = default;
  // PC_PRINT("DESTROYED", TagType::Destr, "VERTEX-BUFFER") };

  virtual void SetData();

protected:
  Buffer<T> m_buffer;
};

}; // namespace Gfx

ENGINE_NAMESPACE_END
