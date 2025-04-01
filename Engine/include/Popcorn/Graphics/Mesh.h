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

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

[[nodiscard]] uint32_t PC_GetHashedSubmeshId(VertexBuffer *vertexBuffer,
                                             IndexBuffer<uint16_t> *indexBuffer,
                                             Material *material);

class Submesh {
public:
  Submesh(VertexBuffer *vertexBuffer, IndexBuffer<uint16_t> *indexBuffer,
          Material *material)
      : m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer),
        m_material(material) {
    PC_PRINT("CREATED", TagType::Constr, "Submesh");
    m_id = PC_GetHashedSubmeshId(vertexBuffer, indexBuffer, material);
  };
  ~Submesh() {
    if (m_vertexBuffer == nullptr || m_indexBuffer == nullptr ||
        m_material == nullptr) {
      PC_WARN("Either of the following are nullptr and so memory not deleted")
      PC_PRINT("DESTROYED", TagType::Destr, "Submesh");

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

  [[nodiscard]] inline const uint32_t GetId() const { return m_id; };

private:
  VertexBuffer *m_vertexBuffer = nullptr;
  IndexBuffer<uint16_t> *m_indexBuffer = nullptr;
  Material *m_material = nullptr;

  uint32_t m_id = 0;
};

class Mesh : public GameObject {
public:
  struct Uniforms {
    alignas(16) glm::mat4 modelMatrix; // Model matrix
  };

  // TODO: Handle the case of duplicating meshes & materials
  // TODO: Make m_indexBuffer a variant
  Mesh() {
    m_uniformBuffer.modelMatrix = m_matrix;
    PC_PRINT("CREATED", TagType::Constr, "MESH");
  };
  ~Mesh() { PC_PRINT("DESTROYED", TagType::Destr, "MESH"); };

  virtual void OnAttach() override;
  virtual void OnUpdate() override;
  virtual void OnRender() override;

  virtual constexpr GameObjectTypes GetType() const override {
    return GameObjectTypes::Mesh;
  };

  [[nodiscard]] inline Uniforms &GetUniformBuffer() { return m_uniformBuffer; };

  void AddSubmesh(VertexBuffer *vertexBuffer,
                  IndexBuffer<uint16_t> *indexBuffer, Material *material) {
    for (auto &submesh : m_submeshes) {
      if (submesh.GetId() ==
          PC_GetHashedSubmeshId(vertexBuffer, indexBuffer, material)) {
        PC_WARN("Identical submesh already exists in Mesh")
        return;
      };
    }

    m_submeshes.emplace_back(vertexBuffer, indexBuffer, material);
  };

protected:
  Uniforms m_uniformBuffer;
  std::vector<Submesh> m_submeshes;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
