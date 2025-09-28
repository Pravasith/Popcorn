#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Buffer.h"
#include "Popcorn/Core/Helpers.h"
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <type_traits>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// --- BUFFER ATTRIBUTE DEFINITIONS ------------------------------------------
//
BUFFER_DEFS_NAMESPACE_BEGIN
// clang-format off
enum class AttrTypes {
  None = 0,
  Float, Float2, Float3, Float4,
  Mat3, Mat4,
  Int, Int2, Int3, Int4,
  Bool
};

static constexpr uint32_t GetAttrTypeSize(BufferDefs::AttrTypes attrType) {
  switch (attrType)
  {
    case AttrTypes::Float:    return 4;
    case AttrTypes::Float2:   return 4 * 2;
    case AttrTypes::Float3:   return 4 * 3;
    case AttrTypes::Float4:   return 4 * 4;
    case AttrTypes::Mat3:     return 4 * 3 * 3;
    case AttrTypes::Mat4:     return 4 * 4 * 4;
    case AttrTypes::Int:      return 4;
    case AttrTypes::Int2:     return 4 * 2;
    case AttrTypes::Int3:     return 4 * 3;
    case AttrTypes::Int4:     return 4 * 4;
    case AttrTypes::Bool:     return 1;
    default:
      PC_ASSERT(false, "Unknown AttrType");
      return 0;
  }
};
// clang-format on

struct Layout {
  std::vector<AttrTypes> attrTypesValue;
  std::vector<uint32_t> attrOffsetsValue{};
  uint32_t strideValue = 0;
  uint32_t countValue = 0;

  // For move constructors
  void Reset() {
    attrTypesValue.clear();
    attrOffsetsValue.clear();
    strideValue = 0;
    countValue = 0;
  };

  bool operator==(const BufferDefs::Layout &other) const {
    return strideValue == other.strideValue && countValue == other.countValue &&
           attrTypesValue == other.attrTypesValue &&
           attrOffsetsValue == other.attrOffsetsValue;
  }

  bool operator!=(const BufferDefs::Layout &other) const {
    return strideValue != other.strideValue || countValue != other.countValue ||
           attrTypesValue != other.attrTypesValue ||
           attrOffsetsValue != other.attrOffsetsValue;
  }

  // template <AttrTypes... E> void Set() {
  //   // Attr Types stored in a seq
  //   (attrTypesValue.push_back(E), ...);
  //
  //   // Attr Offsets stored in a seq (Inital value 0 already exists in the
  //   // array, and we pop the last element offset).
  //   (attrOffsetsValue.push_back(GetAttrTypeSize(E)), ...);
  //   attrOffsetsValue.pop_back();
  //
  //   // Stride & count values
  //   strideValue = (... + GetAttrTypeSize(E));
  //   countValue = sizeof...(E);
  // };

  template <AttrTypes... E> void Set() {
    Reset();

    attrTypesValue = {E...};
    uint32_t offset = 0;

    for (auto type : attrTypesValue) {
      attrOffsetsValue.push_back(offset);
      offset += GetAttrTypeSize(type);
    }

    strideValue = offset;
    countValue = sizeof...(E);
  }
};

BUFFER_DEFS_NAMESPACE_END

//
//
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// --- VERTEX BUFFER ---------------------------------------------------------
//
class VertexBuffer {
public:
  VertexBuffer() {}
  virtual ~VertexBuffer() {}

  template <BufferDefs::AttrTypes... E> inline void SetLayout() {
    m_layout.Set<E...>();
  }

  inline void SetLayout(const BufferDefs::Layout &layout) { m_layout = layout; }

  [[nodiscard]] inline const BufferDefs::Layout &GetLayout() const {
    PC_ASSERT(m_layout.countValue != 0, "BufferDefs::Layout is empty!");
    return m_layout;
  }

  [[nodiscard]] inline byte_t *GetBufferData() const {
    return m_buffer.GetData();
  }

  virtual uint64_t GetSize() const = 0;
  virtual uint64_t GetCount() const = 0;

  template <typename T> void Fill(std::initializer_list<T> list) {
    m_buffer.SetData(list);
  }

  void Fill(const void *data, uint64_t size, uint64_t offset = 0) {
    m_buffer.WriteBytes(data, size, offset);
  }

  template <typename T> void PrintBuffer() { Buffer::Print<T>(m_buffer); }

  static VertexBuffer *Create();
  static void Destroy(VertexBuffer *);

  void Allocate(uint64_t size) { m_buffer.Resize(size); }

  // COPY CONSTRUCTOR
  VertexBuffer(const VertexBuffer &other) {
    PC_PRINT("COPY CONSTRUCTOR EVOKED", TagType::Constr,
             "VERTEX-BUFFER(INHERITED)")
    m_buffer = other.m_buffer;
    m_layout = other.m_layout;
  }
  virtual VertexBuffer &operator=(const VertexBuffer &other) {
    PC_PRINT("COPY ASSIGNMENT EVOKED", TagType::Print,
             "VERTEX-BUFFER(INHERITED)")

    if (this == &other)
      return *this;

    m_buffer = other.m_buffer;
    m_layout = other.m_layout;
    return *this;
  }

  // MOVE CONSTRUCTOR
  VertexBuffer(VertexBuffer &&other) {
    PC_PRINT("MOVE CONSTRUCTOR EVOKED", TagType::Constr,
             "VERTEX-BUFFER(INHERITED)")
    if (this == &other) {
      return;
    };

    m_buffer = std::move(other.m_buffer);
    m_layout = other.m_layout;
    // other.m_layout.Reset();
  }
  virtual VertexBuffer &operator=(VertexBuffer &&other) {
    PC_PRINT("MOVE ASSIGNMENT EVOKED", TagType::Print,
             "VERTEX-BUFFER(INHERITED)")

    if (this == &other) {
      return *this;
    };

    m_buffer = std::move(other.m_buffer);
    m_layout = other.m_layout;
    // other.m_layout.Reset();

    return *this;
  }

protected:
  Buffer m_buffer;
  BufferDefs::Layout m_layout;
};

//
//
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// --- INDEX BUFFER ----------------------------------------------------------
//
template <typename T>
concept Is_Uint16_Or_Uint32_t =
    std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>;

template <Is_Uint16_Or_Uint32_t T> class IndexBuffer {
public:
  IndexBuffer() {}
  ~IndexBuffer() {}

  const uint64_t GetSize() const { return m_buffer.GetSize(); }
  const uint64_t GetCount() const { return m_buffer.GetCount(sizeof(T)); }

  void Allocate(uint64_t size) { m_buffer.Resize(size); }

  void Fill(std::initializer_list<T> elements) { m_buffer.SetData(elements); }

  void Fill(const void *data, uint64_t size, uint64_t offset = 0) {
    m_buffer.WriteBytes(data, size, offset);
  }

  [[nodiscard]] inline byte_t *GetBufferData() const {
    return m_buffer.GetData();
  }

  // COPY CONSTRUCTOR
  IndexBuffer(const IndexBuffer &other) {
    PC_PRINT("COPY CONSTRUCTOR EVOKED", TagType::Constr, "IndexBuffer")
    m_buffer = other.m_buffer;
  }
  virtual IndexBuffer &operator=(const IndexBuffer &other) {
    PC_PRINT("COPY ASSIGNMENT EVOKED", TagType::Print, "IndexBuffer")

    if (this == &other)
      return *this;

    m_buffer = other.m_buffer;
    return *this;
  }

  // MOVE CONSTRUCTOR
  IndexBuffer(IndexBuffer &&other) {
    PC_PRINT("MOVE CONSTRUCTOR EVOKED", TagType::Constr, "INDEX-BUFFER")
    if (this == &other) {
      return;
    }

    m_buffer = std::move(other.m_buffer);
    // other.m_layout.Reset();
  }
  virtual IndexBuffer &operator=(IndexBuffer &&other) {
    PC_PRINT("MOVE ASSIGNMENT EVOKED", TagType::Print, "INDEX-BUFFER")

    if (this == &other) {
      return *this;
    }

    m_buffer = std::move(other.m_buffer);
    // other.m_layout.Reset();

    return *this;
  }

private:
  Buffer m_buffer;
};

//
//
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// --- UNIFORM BUFFER --------------------------------------------------------
//

// class UniformBuffer {
// public:
//   UniformBuffer() { PC_PRINT("CREATED", TagType::Constr, "UniformBuffer") };
//   ~UniformBuffer() { PC_PRINT("DESTROYED", TagType::Destr, "UniformBuffer")
//   };
//
//   template <BufferDefs::AttrTypes... E> inline void SetLayout() {
//     m_layout.Set<E...>();
//   };
//
//   template <typename T> void Fill(std::initializer_list<T> list) {
//     m_buffer.SetData(list);
//   };
//
//   const uint64_t GetSize() const { return m_buffer.GetSize(); };
//   const uint64_t GetCount() const { return m_buffer.GetCount(); };
//
//   [[nodiscard]] inline const BufferDefs::Layout &GetLayout() const {
//     PC_ASSERT(m_layout.countValue != 0, "BufferDefs::Layout is empty!");
//     return m_layout;
//   }
//
//   [[nodiscard]] inline byte_t *GetBufferData() const {
//     return m_buffer.GetData();
//   }
//
//   // COPY CONSTRUCTOR
//   UniformBuffer(const UniformBuffer &other) {
//     PC_PRINT("COPY CONSTRUCTOR EVOKED", TagType::Constr, "UNIFORM-BUFFER")
//     m_buffer = other.m_buffer;
//     m_layout = other.m_layout;
//   };
//
//   virtual UniformBuffer &operator=(const UniformBuffer &other) {
//     PC_PRINT("COPY ASSIGNMENT EVOKED", TagType::Print, "UNIFORM-BUFFER")
//
//     if (this == &other)
//       return *this;
//
//     m_buffer = other.m_buffer;
//     m_layout = other.m_layout;
//     return *this;
//   };
//
//   // MOVE CONSTRUCTOR
//   UniformBuffer(UniformBuffer &&other) {
//     PC_PRINT("MOVE CONSTRUCTOR EVOKED", TagType::Constr, "UNIFORM-BUFFER")
//     if (this == &other) {
//       return;
//     };
//
//     m_buffer = std::move(other.m_buffer);
//     m_layout = other.m_layout;
//   };
//   virtual UniformBuffer &operator=(UniformBuffer &&other) {
//     PC_PRINT("MOVE ASSIGNMENT EVOKED", TagType::Print, "UNIFORM-BUFFER")
//
//     if (this == &other) {
//       return *this;
//     };
//
//     m_buffer = std::move(other.m_buffer);
//     m_layout = other.m_layout;
//
//     return *this;
//   };
//
// private:
//   Buffer m_buffer;
//   BufferDefs::Layout m_layout;
// };

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
