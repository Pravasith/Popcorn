#pragma once

#include "GlobalMacros.h"
#include "Renderer.h"
#include <cstdint>

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

// Buffer class
template <RendererType Rt, ElementType... Args> class Buffer {
public:
  // IMPLEMENT CONSTRUCTOR IN CPP FILE
  Buffer() = default;
  virtual ~Buffer() = default;

  [[nodiscard]] virtual const uint32_t GetSize() const;

  // void Print() const {
  //   for (const auto &element : m_elements) {
  //     PrintElement(element);
  //   }
  // }

private:
  // std::vector<ElementTuple> m_elements;

  // Helper to print tuple contents
  // template <typename... Ts>
  // void PrintElement(const std::tuple<Ts...> &element) const {
  //   std::apply([](const auto &...args) { ((std::cout << args << " "), ...);
  //   },
  //              element);
  //   std::cout << '\n';
  // }
};

}; // namespace Gfx

ENGINE_NAMESPACE_END
