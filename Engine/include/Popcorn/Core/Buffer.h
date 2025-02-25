#pragma once

#include "GlobalMacros.h"
#include "Helpers.h"
#include "Popcorn/Core/Base.h"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <type_traits>

ENGINE_NAMESPACE_BEGIN

template <typename T, uint64_t Count> struct SizeOf {
  static constexpr uint64_t value = sizeof(T) * Count;
};

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

  // TODO: Add memcpy to a new block of contiguous memory logic
  void Resize(uint64_t newSize) { AllocBytes(newSize); }

  Buffer(uint64_t size) {
    PC_PRINT("CREATED(Fixed size runtime)", TagType::Constr, "BUFFER")
    AllocBytes(size);
  };

  template <typename T> void SetData(std::initializer_list<T> list) {
    PC_PRINT("DATA SET(INIT-LIST)", TagType::Print, "BUFFER")

    const uint64_t size = sizeof(T) * list.size();
    AllocBytes(size);

    m_count = list.size();
    m_size = size;
    memcpy(m_data, list.begin(), size);
  };

  [[nodiscard]] inline byte_t *GetData() const {
    return static_cast<byte_t *>(m_data);
  };

  // TODO: FIX THIS
  template <typename T, uint64_t Count = 0>
  Buffer() : m_count(Count), m_size((sizeof(T) * m_count)) {
    PC_PRINT("CREATED", TagType::Constr, "BUFFER")
    AllocBytes(SizeOf<T, Count>::value);
  };

  // TODO: FIX THIS
  template <typename T> Buffer(uint64_t count) : m_count(count) {
    PC_PRINT("CREATED", TagType::Constr, "BUFFER")
    AllocBytes(count * sizeof(T));
  };

  // TODO: FIX THIS
  template <typename T>
  Buffer(T *data, uint64_t count = 0)
      : m_data(data), m_count(count)
        //
        {PC_PRINT("CREATED", TagType::Constr, "BUFFER")};

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
    m_count = other.m_count;

    other.m_data = nullptr;
    other.m_size = 0;
    other.m_count = 0;
  };

  Buffer &&operator=(Buffer &&other) {
    PC_PRINT("MOVE ASSIGNMENT EVOKED", TagType::Print, "BUFFER")
    if (this == &other) {
      return std::move(*this);
    };

    m_data = other.m_data;
    m_size = other.m_size;
    m_count = other.m_count;

    other.m_data = nullptr;
    other.m_size = 0;
    other.m_count = 0;

    // delete (decltype(other.m_data) *)other.m_data;
    return std::move(*this);
  };

  [[nodiscard]] inline const uint64_t GetCount() const { return m_count; };
  [[nodiscard]] inline const uint64_t GetSize() const { return m_size; };

  template <typename T> T *begin() { return static_cast<T *>(m_data); };
  template <typename T> const T *begin() const {
    return static_cast<T *>(m_data);
  };
  template <typename T> T *end() { return static_cast<T *>(m_data) + m_count; };
  template <typename T> const T *end() const {
    return static_cast<T *>(m_data) + m_count;
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
    m_count = 0;
  };

  void *m_data = nullptr;
  uint64_t m_count = 0;
  uint64_t m_size = 0;
};

ENGINE_NAMESPACE_END
