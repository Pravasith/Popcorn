#pragma once

#include "Buffer.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Renderer.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <initializer_list>
#include <tuple>
#include <vector>

ENGINE_NAMESPACE_BEGIN
namespace Gfx {

template <ElementType E> struct ElementMapVk {};

template <> struct ElementMapVk<ElementType::None> {
  // SFINAE
};

template <> struct ElementMapVk<ElementType::Float2> {
  using type = glm::vec2;
};

template <> struct ElementMapVk<ElementType::Float3> {
  using type = glm::vec3;
};

template <ElementType E> using ConvertToType_t = ElementMapVk<E>::type;

template <ElementType... Args>
class VertexBufferVk : public Buffer<RendererType::Vulkan, Args...> {
public:
  using ElementTupleType = std::tuple<ConvertToType_t<Args>...>;

  VertexBufferVk(std::initializer_list<ElementTupleType> elements)
      : m_elements(elements) {
          PC_PRINT("CREATED", TagType::Constr, "VERT-BFR-VK")
        };
  ~VertexBufferVk() {
    m_elements.clear();
    PC_PRINT("DESTROYED", TagType::Destr, "VERT-BFR-VK")
  };

  [[nodiscard]] const uint32_t GetSize() const override {
    return m_elements.size();
  };

private:
  std::vector<ElementTupleType> m_elements;
};

VertexBufferVk<ElementType::Float2, ElementType::Float3> bfr{
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
}; // namespace Gfx
ENGINE_NAMESPACE_END
