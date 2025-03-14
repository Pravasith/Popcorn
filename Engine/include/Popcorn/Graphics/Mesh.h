#pragma once

#include "BufferObjects.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "Renderer.h"
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Mesh : public GameObject {
public:
  struct Uniforms {
    glm::mat4 modelMatrix; // Model matrix
  };

  struct Spec {
    bool enableIndexBuffers = false;
  };

  // TODO: Handle the case of duplicating meshes & materials
  // TODO: Add submeshes & multiple material support
  // TODO: Make m_indexBuffer a variant
  Mesh(VertexBuffer &geometry, IndexBuffer<uint16_t> *indexBuffer,
       Material &material)
      : m_vertexBuffer(geometry), m_indexBuffer(indexBuffer), m_uniformBuffer(),
        m_material(material) {
    if (indexBuffer != nullptr) {
      m_spec.enableIndexBuffers = true;
    };

    s_uniformBufferLayout.Set<BufferDefs::AttrTypes::Mat4>();
    m_uniformBuffer.SetLayout<BufferDefs::AttrTypes::Mat4>();
    m_uniformBuffer.Fill({m_matrix});

    const float *data = glm::value_ptr(m_matrix);
    std::cout << "Model Matrix:\n";
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        std::cout << data[i + j * 4] << " ";
      }
      std::cout << "\n";
    }

    PC_PRINT("CREATED", TagType::Constr, "MESH");
  };
  ~Mesh() { PC_PRINT("DESTROYED", TagType::Destr, "MESH"); };

  void Set(const Spec &spec) {
    ValidateMembersWithSpec(spec);
    m_spec.enableIndexBuffers = spec.enableIndexBuffers;
  };

  void ValidateMembersWithSpec(const Spec &spec);

  virtual void OnAttach() override;
  virtual void OnUpdate() override;
  virtual void OnRender() override;

  virtual constexpr GameObjectTypes GetType() const override {
    return GameObjectTypes::Mesh;
  };

  [[nodiscard]] inline Material &GetMaterial() const { return m_material; };

  [[nodiscard]] inline VertexBuffer &GetVertexBuffer() const {
    return m_vertexBuffer;
  };

  // TODO: Change this to variant return type
  [[nodiscard]] inline IndexBuffer<uint16_t> &GetIndexBuffer() const {
    if (m_indexBuffer == nullptr) {
      PC_ERROR("Trying to get a null m_indexBuffer", "Mesh");
    };
    return *m_indexBuffer;
  };

  [[nodiscard]] inline const UniformBuffer &GetUniformBuffer() const {
    return m_uniformBuffer;
  };

  static const BufferDefs::Layout &GetUniformBufferLayout() {
    return s_uniformBufferLayout;
  };

protected:
  VertexBuffer &m_vertexBuffer;
  IndexBuffer<uint16_t> *m_indexBuffer = nullptr;
  UniformBuffer m_uniformBuffer;

  static BufferDefs::Layout s_uniformBufferLayout;

  // TODO: Make this a Vector as required
  Material &m_material;
  Spec m_spec;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
