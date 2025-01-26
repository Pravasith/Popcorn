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

  // COPY CONSTRUCTOR
  VertexBuffer(const VertexBuffer &other) {
    PC_PRINT("COPY CONSTRUCTOR EVOKED", TagType::Print, "Vertex-Buffer-Vk")
    m_buffer = other.m_buffer;
  };
  VertexBuffer &operator=(const VertexBuffer &other) {
    PC_PRINT("COPY ASSIGNMENT EVOKED", TagType::Print, "Vertex-Buffer-Vk")

    if (this == &other)
      return *this;

    m_buffer = other.m_buffer;
    return *this;
  };

  // MOVE CONSTRUCTOR
  VertexBuffer(VertexBuffer &&other) {
    PC_PRINT("MOVE CONSTRUCTOR EVOKED", TagType::Print, "Vertex-Buffer-Vk")
    if (this == &other) {
      return;
    };
    m_buffer = other.m_buffer;
  };
  VertexBuffer &&operator=(VertexBuffer &&other) {
    PC_PRINT("MOVE ASSIGNMENT EVOKED", TagType::Print, "Vertex-Buffer-Vk")

    if (this == &other) {
      return std::move(*this);
    };

    m_buffer = std::move(other.m_buffer);
    return std::move(*this);
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
