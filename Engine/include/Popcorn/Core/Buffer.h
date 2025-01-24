
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

// BASE TEMPLATE
template <typename T, typename V = void> struct HasPrint : std::false_type {};

// SPECIALIZATION
template <typename T>
struct HasPrint<
    T,
    // FALSE - SUBSTITUTION FAILURE BC ::enable_if<bool>::type
    // DOESN'T EXIST TRUE - value IS THE SAME TYPE AS THE 2ND PARAM,
    // WHICH IS NOT DEFINED IN THIS CASE, SO VOID??
    typename std::enable_if<
        //
        std::is_member_function_pointer<decltype(&T::Print)>::value>::type
    //
    > : std::true_type {};

class Buffer {
public:
  Buffer() = default;

  template <typename T> void SetData(std::initializer_list<T> list) {
    PC_PRINT("CREATED(INIT-LIST)", TagType::Constr, "BUFFER")

    const uint64_t size = sizeof(T) * list.size();
    FreeBytes();
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

  Buffer(const Buffer &other, const uint64_t size) {
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

  ~Buffer() { FreeBytes(); };

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
    for (T *it = buffer.begin<T>(); it != buffer.end<T>(); ++it) {
      // TODO: CHECK IF METHOD (Print IN THIS CASE) EXISTS -- IMPLEMENT A SFINAE
      // HELPER
      // https://www.reddit.com/r/cpp_questions/comments/pbh8zb/sfinae_detect_if_method_exists_cpp17/
      if constexpr (HasPrint<T>()) {
        PC_PRINT(
            // BELOW STATEMENT
            ((T &)(*it)).Print(),
            // IS EQUIVALENT TO:
            // elem.Print(),
            // WHERE: T &elem = *it;
            TagType::Print, "BUFFER")
      };
    }
  };
#else
  static void Print(Buffer &buffer) {};
#endif

  template <typename T> T *AsType() { return (T *)m_data; };

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

ENGINE_NAMESPACE_END
