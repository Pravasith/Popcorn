#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "Renderer.h"
#include "BufferObjects.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Mesh : public GameObject {
public:
  struct Spec {
    bool enableIndexBuffers = false;
  };

  // TODO: Handle the case of duplicating meshes & materials
  // TODO: Add submeshes & multiple material support
  // TODO: Make m_indexBuffer a variant
  Mesh(VertexBuffer &geometry, IndexBuffer<uint16_t> *indexBuffer,
       Material &material)
      : m_vertexBuffer(geometry), m_indexBuffer(indexBuffer),
        m_material(material) {
    if (indexBuffer != nullptr) {
      m_spec.enableIndexBuffers = true;
    };

    PC_PRINT("CREATED", TagType::Constr, "MESH");
  };
  ~Mesh() { PC_PRINT("DESTROYED", TagType::Destr, "MESH"); };

  void Set(const Spec &spec) {
    ValidateMembersWithSpec(spec);
    m_spec.enableIndexBuffers = spec.enableIndexBuffers;
  };

  void ValidateMembersWithSpec(const Spec &spec);

  virtual void OnAttach(const SceneData &) override;
  virtual void OnUpdate(const SceneData &) override;
  virtual void OnRender(const SceneData &) override;

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

protected:
  VertexBuffer &m_vertexBuffer;
  IndexBuffer<uint16_t> *m_indexBuffer = nullptr;
  // TODO: Make this a Vector as required
  Material &m_material;
  Spec m_spec;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
