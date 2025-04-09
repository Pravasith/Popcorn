#pragma once

#include "BufferObjects.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// ----------------------------------------------------------------------------
// --- UTILS ------------------------------------------------------------------
template <MaterialTypes T>
[[nodiscard]] uint32_t PC_GetHashedSubmeshId(VertexBuffer *vertexBuffer,
                                             IndexBuffer<uint16_t> *indexBuffer,
                                             Material<T> *material) {
  uintptr_t vAddr = reinterpret_cast<uintptr_t>(vertexBuffer);
  uintptr_t iAddr = reinterpret_cast<uintptr_t>(indexBuffer);
  uintptr_t mAddr = reinterpret_cast<uintptr_t>(material);

  return static_cast<uint32_t>((vAddr ^ (iAddr >> 3) ^ (mAddr >> 6)) *
                               2654435761u); // Knuth's golden ratio
}

//
//
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// --- SUBMESH ----------------------------------------------------------------
//
template <MaterialTypes T> class Submesh {
public:
  Submesh(VertexBuffer *vbo, IndexBuffer<uint16_t> *ibo, Material<T> *mat)
      : m_vertexBuffer(vbo), m_indexBuffer(ibo), m_material(mat) {
    PC_PRINT("CREATED", TagType::Constr, "Submesh");
    m_id = PC_GetHashedSubmeshId(vbo, ibo, mat);
  }
  ~Submesh() {
    Cleanup();
    PC_PRINT("DESTROYED", TagType::Destr, "Submesh");
  }

  // DELETE COPY CONSTRUCTOR AND ASSIGNMENT
  // Because this class deletes vbo, ibo, and mat data, disabling copy avoids
  // dangling pointers
  Submesh(const Submesh &) = delete;
  Submesh &operator=(const Submesh &) = delete;

  //
  // --- MOVE CONSTRUCTOR -----------------------------------------------------
  Submesh(Submesh &&other) noexcept
      : m_vertexBuffer(other.m_vertexBuffer),
        m_indexBuffer(other.m_indexBuffer), m_material(other.m_material),
        m_id(other.m_id) {
    other.m_vertexBuffer = nullptr;
    other.m_indexBuffer = nullptr;
    other.m_material = nullptr;
  }

  //
  // --- MOVE ASSIGNMENT ------------------------------------------------------
  Submesh &operator=(Submesh &&other) noexcept {
    if (this != &other) {
      Cleanup();

      m_vertexBuffer = other.m_vertexBuffer;
      m_indexBuffer = other.m_indexBuffer;
      m_material = other.m_material;
      m_id = other.m_id;

      other.m_vertexBuffer = nullptr;
      other.m_indexBuffer = nullptr;
      other.m_material = nullptr;
    }
    return *this;
  }

  [[nodiscard]] uint32_t GetId() const { return m_id; }

private:
  void Cleanup() {
    if (m_vertexBuffer)
      delete m_vertexBuffer;
    if (m_indexBuffer)
      delete m_indexBuffer;
    if (m_material)
      delete m_material;
  }

  VertexBuffer *m_vertexBuffer = nullptr;
  IndexBuffer<uint16_t> *m_indexBuffer = nullptr;
  Material<T> *m_material = nullptr;
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
    m_uniformBuffer.modelMatrix = m_worldMatrix;
    PC_PRINT("CREATED", TagType::Constr, "MESH");
  };
  virtual ~Mesh() { PC_PRINT("DESTROYED", TagType::Destr, "MESH"); };

  // virtual void OnAttach() override {};
  virtual void OnUpdate() override {
    // Resets buffer data and fills again
    m_uniformBuffer.modelMatrix = GetWorldMatrix();
  };
  // virtual void OnRender() override {};

  virtual constexpr GameObjectTypes GetType() const override {
    return GameObjectTypes::Mesh;
  };

  [[nodiscard]] inline Uniforms &GetUniformBuffer() { return m_uniformBuffer; };

  template <MaterialTypes T>
  void AddSubmesh(VertexBuffer *vbo, IndexBuffer<uint16_t> *ibo,
                  Material<T> *mat) {
    PC_ASSERT(vbo && ibo && mat, "Submesh parameter(s) nullptr");

    if constexpr (T == MaterialTypes::BasicMat) {
      for (auto &submesh : m_basicSubmeshes) {
        if (submesh.GetId() == PC_GetHashedSubmeshId(vbo, ibo, mat)) {
          PC_WARN("Identical submesh already exists in Mesh")
          return;
        };
      }

      m_basicSubmeshes.emplace_back(vbo, ibo, mat);
    } else if constexpr (T == MaterialTypes::PbrMat) {
      for (auto &submesh : m_pbrSubmeshes) {
        if (submesh.GetId() == PC_GetHashedSubmeshId(vbo, ibo, mat)) {
          PC_WARN("Identical submesh already exists in Mesh")
          return;
        };
      }

      m_pbrSubmeshes.emplace_back(vbo, ibo, mat);
    } else {
      // Rest material types...
    }
  };

  template <MaterialTypes T>
  [[nodiscard]]
  std::vector<Submesh<T>> &GetSubmeshes() {
    if constexpr (T == MaterialTypes::BasicMat) {
      return m_basicSubmeshes;
    } else if constexpr (T == MaterialTypes::PbrMat) {
      return m_pbrSubmeshes;
    };
  };

protected:
  Uniforms m_uniformBuffer;

  //
  std::vector<Submesh<MaterialTypes::BasicMat>> m_basicSubmeshes;
  std::vector<Submesh<MaterialTypes::PbrMat>> m_pbrSubmeshes;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
