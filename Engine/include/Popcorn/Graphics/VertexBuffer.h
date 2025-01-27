#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include <cstdint>
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
    // m_vertexBfr = nullptr
    PC_PRINT("CREATED(DEFAULT)", TagType::Constr, "VERTEX-BUFFER");
  };
  virtual ~VertexBuffer() {
    PC_PRINT("DESTROYED", TagType::Destr, "VERTEX-BUFFER")
  };

  template <typename T> void Fill(std::initializer_list<T> list) {
    m_buffer.SetData(list);
  };

  // COPY CONSTRUCTOR
  VertexBuffer(const VertexBuffer &other) {
    PC_PRINT("Inherited COPY CONSTRUCTOR EVOKED", TagType::Print,
             "Vertex-Buffer-Vk")
    m_buffer = other.m_buffer;
  };
  virtual VertexBuffer &operator=(const VertexBuffer &other) {
    PC_PRINT("Inherited COPY ASSIGNMENT EVOKED", TagType::Print,
             "Vertex-Buffer-Vk")

    if (this == &other)
      return *this;

    m_buffer = other.m_buffer;
    return *this;
  };

  // MOVE CONSTRUCTOR
  VertexBuffer(VertexBuffer &&other) {
    PC_PRINT("Inherited MOVE CONSTRUCTOR EVOKED", TagType::Print,
             "Vertex-Buffer-Vk")
    if (this == &other) {
      return;
    };

    m_buffer = std::move(other.m_buffer);
  };
  virtual VertexBuffer &operator=(VertexBuffer &&other) {
    PC_PRINT("Inherited MOVE ASSIGNMENT EVOKED", TagType::Print,
             "Vertex-Buffer-Vk")

    if (this == &other) {
      return *this;
    };

    m_buffer = std::move(other.m_buffer);

    return *this;
  };

  static VertexBuffer *Create();

  virtual uint64_t GetSize() const = 0;
  virtual uint64_t GetCount() const = 0;

  virtual void Bind() = 0;
  virtual void UnBind() = 0;

  template <typename T> void PrintBuffer() { Buffer::Print<T>(m_buffer); };

protected:
  Buffer m_buffer;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
