#pragma once

#include "GlobalMacros.h"
#include <glm/glm.hpp>
#include <initializer_list>
#include <iostream>
#include <tuple>
#include <vector>

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

// Map ElementType to its corresponding type
template <ElementType E> struct ConvertToType;

template <> struct ConvertToType<ElementType::Float2> {
  using type = glm::vec2;
};

template <> struct ConvertToType<ElementType::Float3> {
  using type = glm::vec3;
};

// Helper alias
template <ElementType E>
using ConvertToType_t = typename ConvertToType<E>::type;

// Buffer class
template <ElementType... Args> class Buffer {
public:
  using ElementTuple = std::tuple<ConvertToType_t<Args>...>;

  Buffer(std::initializer_list<ElementTuple> elements) : m_elements(elements) {}

  void Print() const {
    for (const auto &element : m_elements) {
      PrintElement(element);
    }
  }

private:
  std::vector<ElementTuple> m_elements;

  // Helper to print tuple contents
  template <typename... Ts>
  void PrintElement(const std::tuple<Ts...> &element) const {
    std::apply([](const auto &...args) { ((std::cout << args << " "), ...); },
               element);
    std::cout << '\n';
  }
};

// Buffer<ElementType::Float2, ElementType::Float3> bfr = {
//     {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
//     {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

}; // namespace Gfx

ENGINE_NAMESPACE_END
