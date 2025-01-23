#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include "Renderer.h"
#include <cassert>
#include <cstring>
#include <initializer_list>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

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

class VertexBuffer {
public:
  VertexBuffer() { PC_PRINT("CREATED", TagType::Constr, "VERTEX-BUFFER") };
  template <typename T> void Fill(std::initializer_list<T> list) {
    m_buffer.SetData(list);
  };
  virtual ~VertexBuffer();

  void Init() const;

  template <typename T> void PrintBuffer() { Buffer::Print<T>(m_buffer); };

protected:
  Buffer m_buffer;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
