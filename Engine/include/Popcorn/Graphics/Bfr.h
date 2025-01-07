#pragma once

#include "GlobalMacros.h"
#include <glm/glm.hpp>
#include <initializer_list>
#include <tuple>
#include <vector>

ENGINE_NAMESPACE_BEGIN
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

template <ElementType E> struct ElementMap {};

template <> struct ElementMap<ElementType::None> {
  // SFINAE
};

// DEFINE THIS IN INHERITED CLASS
template <> struct ElementMap<ElementType::Float2> {
  // DEFINE CUSTOM TYPE: USING GLM FOR VULKAN, MAYBE SOMETHING ELSE FOR OPENGL,
  // DIRECTX ...ETC
  using type = glm::vec3;
};

// template <> struct ElementMap<ElementType::Float3> {
//   // DEFINE CUSTOM TYPE: USING GLM FOR VULKAN, MAYBE SOMETHING ELSE FOR
//   OPENGL,
//   // DIRECTX ...ETC
//   using type = glm::vec3;
// };

template <ElementType E> using ConvertToType_t = ElementMap<E>::type;

template <ElementType... Args> class Buffer {
public:
  using ElementTupleType = std::tuple<ConvertToType_t<Args>...>;

  Buffer(std::initializer_list<ElementTupleType> elements)
      : m_elements(elements) {};
  ~Buffer() {};

private:
  std::vector<ElementTupleType> m_elements;
};

// Buffer<ElementType::Float2, ElementType::Float3> bfr = {
//     {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//     {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
//     {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

ENGINE_NAMESPACE_END
