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
  Mesh(VertexBuffer &geometry, Material &material)
      : m_vertexBuffer(geometry), m_material(material) {
    PC_PRINT("CREATED", TagType::Constr, "MESH");
  };
  ~Mesh() { PC_PRINT("DESTROYED", TagType::Destr, "MESH"); };

  // void AddMaterial(Material &material) {
  //
  // };

  virtual void OnAttach(const SceneData &) override {
    // TODO: inject vertices from outside this class instead
  };
  virtual void OnUpdate(const SceneData &) override {};
  virtual void OnRender(const SceneData &) override {
    // if (m_material == nullptr) {
    //   PC_WARN("m_material is nullptr");
    //   return;
    // }

    // FIND MATERIALS & RENDER THEM WITH THE RENDERPASS INFORMATION
  };

private:
  VertexBuffer &m_vertexBuffer;
  // TODO: Make this a Vector as required
  Material &m_material;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
