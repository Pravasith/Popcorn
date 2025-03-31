#pragma once

#include "BufferObjects.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include <algorithm>
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Mesh : public GameObject {
public:
  struct Uniforms {
    alignas(16) glm::mat4 modelMatrix; // Model matrix
  };

  // TODO: Handle the case of duplicating meshes & materials
  // TODO: Make m_indexBuffer a variant
  Mesh(VertexBuffer *vertexBuffer, IndexBuffer<uint16_t> *indexBuffer,
       Material *material)
      : m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer),
        m_material(material), m_uniformBuffer() {
    m_uniformBuffer.modelMatrix = m_matrix;
    PC_PRINT("CREATED", TagType::Constr, "MESH");
  };
  ~Mesh() {
    if (m_vertexBuffer == nullptr || m_indexBuffer == nullptr ||
        m_material == nullptr) {
      PC_WARN("Either of the following are nullptr and so memory not deleted")
      PC_PRINT("DESTROYED", TagType::Destr, "MESH");
      return;
    };

    delete m_vertexBuffer;
    m_vertexBuffer = nullptr;

    delete m_indexBuffer;
    m_indexBuffer = nullptr;

    delete m_material;
    m_material = nullptr;

    PC_PRINT("DESTROYED", TagType::Destr, "MESH");
  };

  virtual void OnAttach() override;
  virtual void OnUpdate() override;
  virtual void OnRender() override;

  virtual constexpr GameObjectTypes GetType() const override {
    return GameObjectTypes::Mesh;
  };

  [[nodiscard]] inline const Material &GetMaterial() const {
    if (m_material == nullptr) {
      PC_ERROR("Trying to get a null m_material", "Mesh");
    };
    return *m_material;
  };

  [[nodiscard]] inline VertexBuffer &GetVertexBuffer() const {
    if (m_vertexBuffer == nullptr) {
      PC_ERROR("Trying to get a null m_vertexBuffer", "Mesh");
    };
    return *m_vertexBuffer;
  };

  // TODO: Change this to variant return type
  [[nodiscard]] inline IndexBuffer<uint16_t> &GetIndexBuffer() const {
    if (m_indexBuffer == nullptr) {
      PC_ERROR("Trying to get a null m_indexBuffer", "Mesh");
    };
    return *m_indexBuffer;
  };

  [[nodiscard]] inline Uniforms &GetUniformBuffer() { return m_uniformBuffer; };

protected:
  VertexBuffer *m_vertexBuffer = nullptr;
  IndexBuffer<uint16_t> *m_indexBuffer = nullptr;
  Material *m_material = nullptr;

  Uniforms m_uniformBuffer;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
