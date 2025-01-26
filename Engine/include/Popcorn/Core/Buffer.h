#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <initializer_list>
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
  Buffer() {
    AllocBytes(0);
    PC_PRINT("CREATED(DEFAULT)", TagType::Constr, "BUFFER")
  };
  ~Buffer() {

    PC_PRINT("FREE CALLED !!!!!!!!     ", TagType::Destr, "BUFFER")
    FreeBytes();
    PC_PRINT("DESTROYED", TagType::Destr, "BUFFER")
  };

  template <typename T> void SetData(std::initializer_list<T> list) {
    PC_PRINT("CREATED(INIT-LIST)", TagType::Constr, "BUFFER")

    const uint64_t size = sizeof(T) * list.size();
    AllocBytes(size);
    // THESE LINES HERE DON'T WORK AS EXPECTED. THE m_count IS ZERO??
    m_count = list.size();
    m_size = size;
    memcpy(m_data, list.begin(), size);
    // m_count = list.size();
    // m_size = size;
  };

  template <typename T, uint64_t Count = 0>
  Buffer() : m_count(Count), m_size((sizeof(T) * m_count)) {
    PC_PRINT("CREATED", TagType::Constr, "BUFFER")
    AllocBytes(SizeOf<T, Count>::value);
  };

  template <typename T> Buffer(uint64_t count) : m_count(count) {
    PC_PRINT("THIS_CREATED", TagType::Constr, "BUFFER")
    AllocBytes(count * sizeof(T));
  };
  template <typename T>
  Buffer(T *data, uint64_t count = 0)
      : m_data(data),
        m_count(count){PC_PRINT("CREATED", TagType::Constr, "BUFFER")};

  // COPY CONSTRUCTOR ------------------------------------------------------
  Buffer(const Buffer &other) {
    PC_PRINT("COPY CONSTRUCTOR EVOKED", TagType::Print, "BUFFER")
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

  // MOVE CONSTRUCTOR ------------------------------------------------------
  Buffer(Buffer &&other) {
    PC_PRINT("MOVE CONSTRUCTOR EVOKED", TagType::Print, "BUFFER")
    if (this == &other) {
      return;
    };

    m_data = other.m_data;
  };
  Buffer &&operator=(Buffer &&other) {
    PC_PRINT("MOVE ASSIGNMENT EVOKED", TagType::Print, "BUFFER")
    if (this == &other) {
      return std::move(*this);
    };

    m_data = other.m_data;
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
    if constexpr (HasPrint<T>()) {
      for (T *it = buffer.begin<T>(); it != buffer.end<T>(); ++it) {
        PC_PRINT(((T &)(*it)).Print(), TagType::Print, "BUFFER")
      };
    }
  };
#else
  static void Print(Buffer &buffer) {};
#endif

  template <typename T> T *AsType() { return (T *)m_data; };

private:
  void AllocBytes(uint64_t size) {
    PC_PRINT(m_data, TagType::Print, "BEFORE-ALLOC")
    FreeBytes();

    // if (size == 0)
    //   return;

    m_data = new byte_t[size];
    m_size = size;
    PC_PRINT(m_data, TagType::Print, "AFTER-ALLOC")
  };

  void FreeBytes() {
    PC_PRINT(m_data, TagType::Print, "BEFORE-FREE")
    if (m_data)
      delete[] static_cast<byte_t *>(m_data);
    m_data = nullptr;
    PC_PRINT(m_data, TagType::Print, "AFTER-FREE")

    m_size = 0;
    m_count = 0;
  };

  void *m_data = nullptr;
  uint64_t m_count = 0;
  uint64_t m_size = 0;
};

ENGINE_NAMESPACE_END
