#pragma once

#include "BufferObjects.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
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

  struct Spec {
    bool enableIndexBuffers = false;
  };

  // TODO: Handle the case of duplicating meshes & materials
  // TODO: Add submeshes & multiple material support
  // TODO: Make m_indexBuffer a variant
  Mesh(VertexBuffer *vertexBuffer, IndexBuffer<uint16_t> *indexBuffer)
      : m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer),
        m_uniformBuffer() {
    if (indexBuffer != nullptr) {
      m_spec.enableIndexBuffers = true;
    };

    m_uniformBuffer.modelMatrix = m_matrix;
    PC_PRINT("CREATED", TagType::Constr, "MESH");
  };
  ~Mesh() {
    // TODO: Add clean up logic
    PC_PRINT("DESTROYED", TagType::Destr, "MESH");
  };

  void Set(const Spec &spec) {
    ValidateMembersWithSpec(spec);
    m_spec.enableIndexBuffers = spec.enableIndexBuffers;
  };

  void SetModelMatrix(glm::mat4 modelMatrix) { m_matrix = modelMatrix; };

  void ValidateMembersWithSpec(const Spec &spec);

  virtual void OnAttach() override;
  virtual void OnUpdate() override;
  virtual void OnRender() override;

  virtual constexpr GameObjectTypes GetType() const override {
    return GameObjectTypes::Mesh;
  };

  template <MaterialTypes T>
  [[nodiscard]] inline std::vector<Material<T> *> &GetMaterialsByType() {
    if constexpr (T == MaterialTypes::BasicMat) {
      return m_basicMaterials;
    }

    // TODO: Add other materials
    else if constexpr (T == MaterialTypes::PbrMat) {
      return {};
    } else {
      PC_ERROR("Material type not found!", "Mesh")
      return {};
    }
  };

  template <MaterialTypes T> void AttachMaterial(Material<T> *materialPtr) {
    PC_AddMaterialByType(materialPtr, m_basicMaterials);
  }

  template <MaterialTypes T> void DetachMaterial(Material<T> *materialPtr) {
    PC_RemoveMaterialByType(materialPtr, m_basicMaterials);
  }

  [[nodiscard]] inline VertexBuffer &GetVertexBuffer() const {
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
  VertexBuffer *m_vertexBuffer;
  IndexBuffer<uint16_t> *m_indexBuffer = nullptr;
  Uniforms m_uniformBuffer;

  // TODO: Add other materials
  std::vector<Material<MaterialTypes::BasicMat> *> m_basicMaterials;

  Spec m_spec;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
