#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
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
  VertexBuffer() {
    // m_vertexBfr ??
    PC_PRINT("CREATED", TagType::Constr, "VERTEX-BUFFER")
  };
  virtual ~VertexBuffer() {
    if (m_vertexBfr)
      delete m_vertexBfr;
    m_vertexBfr = nullptr;

    PC_PRINT("DESTROYED", TagType::Destr, "VERTEX-BUFFER")
  };

  template <typename T> void Fill(std::initializer_list<T> list) noexcept {
    m_buffer.SetData(list);
    m_vertexBfr = Init();
  };

  [[nodiscard]] inline const uint64_t GetSize() const {
    return m_buffer.GetSize();
  };

  [[nodiscard]] inline const uint64_t GetCount() const {
    return m_buffer.GetCount();
  };

  // [[nodiscard]] inline const unsigned int GetVertexSize() const {
  //   PC_ASSERT(m_buffer.GetCount() == 0, "Buffer count is zero!");
  //   return m_buffer.GetSize() / m_buffer.GetCount();
  // };

  virtual void Bind() {};
  virtual void UnBind() {};

  template <typename T> void PrintBuffer() { Buffer::Print<T>(m_buffer); };

private:
  VertexBuffer *Init() const;

protected:
  VertexBuffer *m_vertexBfr = nullptr;
  Buffer m_buffer;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
