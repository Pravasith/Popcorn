#pragma once

#include "GlobalMacros.h"
#include "Helpers.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <type_traits>

ENGINE_NAMESPACE_BEGIN

// PRIMARY TEMPLATE
template <typename T, typename V = void> struct HasPrint : std::false_type {};

// SPECIALIZATION
template <typename T>
struct HasPrint<
    T,
    // IF std::enable_if 1ST PARAM IS -
    // FALSE - SUBSTITUTION FAILURE BC ::enable_if<bool>::type DOESN'T EXIST.
    // TRUE - value IS THE SAME TYPE AS THE 2ND PARAM,
    // WHICH IS NOT DEFINED IN THIS CASE, SO RETURNS void.
    typename std::enable_if<
        //
        std::is_member_function_pointer<decltype(&T::Print)>::value>::type
    //
    > : std::true_type {};

class Buffer {
public:
  Buffer() { PC_PRINT("CREATED(DEFAULT)", TagType::Constr, "BUFFER") };
  ~Buffer() {
    FreeBytes();
    PC_PRINT("DESTROYED", TagType::Destr, "BUFFER")
  };

  //
  // -----------------------------------------------------------------------
  // --- MEMORY ALLOCATION -------------------------------------------------
  void Resize(uint64_t size) {
    //
    AllocBytes(size);
  }

  Buffer(uint64_t size) {
    PC_PRINT("CREATED(Fixed size runtime)", TagType::Constr, "BUFFER")
    AllocBytes(size);
  };

  template <typename T> void SetData(std::initializer_list<T> list) {
    // PC_PRINT("DATA SET(INIT-LIST)", TagType::Print, "BUFFER")

    const uint64_t size = sizeof(T) * list.size();
    // PC_WARN(size << " SIZE")
    AllocBytes(size);

    memcpy(m_data, list.begin(), size);
    m_size = size;
  };

  [[nodiscard]] inline byte_t *GetData() const {
    return static_cast<byte_t *>(m_data);
  };

  void WriteBytes(const void *data, uint64_t size, uint64_t offset = 0) {
    PC_ASSERT(offset + size <= m_size, "WriteBytes out of bounds!");
    memcpy(static_cast<byte_t *>(m_data) + offset, data, size);
  }

  //
  // -----------------------------------------------------------------------
  // --- COPY SEMANTICS ----------------------------------------------------
  Buffer(const Buffer &other) {
    PC_PRINT("COPY CONSTRUCTOR EVOKED", TagType::Constr, "BUFFER")
    if (this == &other)
      return;

    size_t size = other.m_size;
    AllocBytes(size);
    memcpy(m_data, other.m_data, size);
  };

  Buffer &operator=(const Buffer &other) {
    PC_PRINT("COPY ASSIGNMENT EVOKED", TagType::Print, "BUFFER")

    if (this == &other)
      return *this;

    size_t size = other.m_size;
    AllocBytes(size);
    memcpy(m_data, other.m_data, size);

    return *this;
  };

  //
  // -----------------------------------------------------------------------
  // --- MOVE SEMANTICS ----------------------------------------------------
  Buffer(Buffer &&other) {
    PC_PRINT("MOVE CONSTRUCTOR EVOKED", TagType::Constr, "BUFFER")
    if (this == &other) {
      return;
    };

    m_data = other.m_data;
    m_size = other.m_size;

    other.m_data = nullptr;
    other.m_size = 0;
  };

  Buffer &operator=(Buffer &&other) {
    PC_PRINT("MOVE ASSIGNMENT EVOKED", TagType::Print, "BUFFER")
    if (this == &other) {
      return *this;
    };

    m_data = other.m_data;
    m_size = other.m_size;

    other.m_data = nullptr;
    other.m_size = 0;

    // delete (decltype(other.m_data) *)other.m_data;
    return *this;
  };

  [[nodiscard]] const uint64_t GetCount(const uint64_t offset) const {
    PC_ASSERT(offset > 0, "Offset must be greater than zero!");
    return m_size / offset;
  };

  [[nodiscard]] const uint64_t GetSize() const { return m_size; };

  // Iterations
  template <typename T> T *begin() { return static_cast<T *>(m_data); };
  template <typename T> const T *begin() const {
    return static_cast<T *>(m_data);
  };
  template <typename T> T *end() {
    return static_cast<T *>(m_data) + (m_size / sizeof(T));
  };
  template <typename T> const T *end() const {
    return static_cast<T *>(m_data) + (m_size / sizeof(T));
  };

#ifdef PC_DEBUG
  template <typename T> inline static void Print(Buffer &buffer) {
    PC_PRINT("PRINT EVOKED", TagType::Print, "BUFFER")

    if (!buffer.m_data) {
      PC_PRINT("NO DATA IN BUFFER", TagType::Print, "Buffer.h")
      return;
    };

    if constexpr (HasPrint<T>()) {
      if (buffer.begin<T>() == buffer.end<T>()) {
        PC_PRINT("Buffer iterator begin is same as end!", TagType::Print,
                 "Buffer.h")
      };

      for (T *it = buffer.begin<T>(); it != buffer.end<T>(); ++it) {
        PC_PRINT(((T &)(*it)).Print(), TagType::Print, "BUFFER")
      };
    } else {
      PC_PRINT("PRINT EVOKED IN BYTES", TagType::Print, "BUFFER")
      std::cout << buffer.begin<char>();
    };
  };
#else
  static void Print(Buffer &buffer) {};
#endif

  template <typename T> T *AsType() const { return static_cast<T *>(m_data); };

private:
  void AllocBytes(uint64_t size) {
    FreeBytes();

    m_data = new byte_t[size];
    m_size = size;
  };

  void FreeBytes() {
    if (m_data)
      delete[] static_cast<byte_t *>(m_data);
    m_data = nullptr;
    m_size = 0;
  };

  void *m_data = nullptr;
  uint64_t m_size = 0;
};

ENGINE_NAMESPACE_END
