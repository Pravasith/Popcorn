#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <glm/glm.hpp>

ENGINE_NAMESPACE_BEGIN
namespace Gfx {

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

template <typename T>
concept ElementTypesConcept = std::same_as<T, ElementType>;

template <ElementTypesConcept... Args> class BufferElement {
public:
  BufferElement(Args... args) {
    PC_PRINT("CREATED", TagType::Constr, "VERT-BFR")
  };
  virtual ~BufferElement() {
    PC_PRINT("DESTROYED", TagType::Destr, "VERT-BFR")
  };

  virtual void Alloc() = 0;

private:
  // CONSTRUCT A PROPER DATA MEMBER USING constexprs WITH Args
};

}; // namespace Gfx

ENGINE_NAMESPACE_END
