#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cassert>
#include <cstdint>
#include <cstring>

ENGINE_NAMESPACE_BEGIN
namespace Gfx {

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

template <typename T, uint64_t Count> struct SizeOf {
  static constexpr uint64_t value = sizeof(T) * Count;
};

template <typename T, uint64_t Count = 0> class Buffer {
public:
  using type = T;

  Buffer() {
    AllocBytes(SizeOf<T, Count>::value);
    m_count = Count;
  };
  Buffer(uint64_t count) : m_count(count) { AllocBytes(count * sizeof(T)); };
  Buffer(T *data, uint64_t count = 0) : m_data(data), m_count(count) {};

  Buffer(const Buffer &other) {
    size_t size = sizeof(T) * other.m_count;
    AllocBytes(size);
    memcpy(m_data, other.m_data, size);
  };

  Buffer &operator=(const Buffer &other) {
    if (this == &other)
      return *this;

    size_t size = sizeof(T) * other.m_count;
    AllocBytes(size);
    memcpy(m_data, other.m_data, size);

    return *this;
  };

  Buffer(const Buffer &other, uint64_t size) {
    AllocBytes(size);
    memcpy(m_data, other.m_data, size);
  };

  ~Buffer() { FreeBytes(); };

  [[nodiscard]] inline const uint64_t GetCount() const { return m_count; };
  [[nodiscard]] inline const uint64_t GetSize() const { return m_size; };

private:
  void AllocBytes(uint64_t size) {
    FreeBytes();

    if (size == 0)
      return;

    m_data = new byte_t[size];
    m_size = size;
  };

  void FreeBytes() {
    delete[] static_cast<byte_t *>(m_data);
    m_data = nullptr;
    m_size = 0;
    m_count = 0;
  };

  void *m_data = nullptr;
  uint64_t m_count = 0;
  uint64_t m_size = 0;
};

class VertexBuffer {
  VertexBuffer();
  ~VertexBuffer();
};

}; // namespace Gfx

ENGINE_NAMESPACE_END
