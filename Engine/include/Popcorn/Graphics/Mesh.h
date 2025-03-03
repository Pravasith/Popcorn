#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "Renderer.h"
#include "VertexBuffer.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Mesh : public GameObject {
public:
  // TODO: Handle the case of duplicating meshes & materials
  // TODO: Add submeshes & multiple material support
  Mesh(VertexBuffer &geometry, Material &material)
      : m_vertexBuffer(geometry), m_material(material) {
    // Add a mesh reference back to material
    m_material.LinkMesh(this);
    PC_PRINT("CREATED", TagType::Constr, "MESH");
  };

  ~Mesh() {
    // Remove the mesh reference to material
    m_material.UnlinkMesh(this);
    PC_PRINT("DESTROYED", TagType::Destr, "MESH");
  };

  virtual void OnAttach(const SceneData &) override;
  virtual void OnUpdate(const SceneData &) override;
  virtual void OnRender(const SceneData &) override;

  virtual constexpr GameObjectTypes GetType() const override {
    return GameObjectTypes::Mesh;
  };

  [[nodiscard]] inline Material &GetMaterial() const { return m_material; };

protected:
  VertexBuffer &m_vertexBuffer;
  // TODO: Make this a Vector as required
  Material &m_material;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
