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
                                             IndexBuffer<uint32_t> *indexBuffer,
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
  Submesh(Mesh *parentMesh, VertexBuffer *vbo, IndexBuffer<uint32_t> *ibo,
          Material<T> *mat)
      : m_parentMesh(parentMesh), m_vertexBuffer(vbo), m_indexBuffer(ibo),
        m_material(mat) {
    PC_PRINT("CREATED", TagType::Constr, "Submesh");
    m_id = PC_GetHashedSubmeshId(vbo, ibo, mat);
  }
  ~Submesh() {
    Cleanup();
    PC_PRINT("DESTROYED", TagType::Destr, "Submesh");
  }

  [[nodiscard]] Material<T> *GetMaterial() const { return m_material; };
  [[nodiscard]] Mesh *GetParentMesh() const { return m_parentMesh; };

  static constexpr MaterialTypes type_value = T;

  [[nodiscard]] VertexBuffer *GetVertexBuffer() { return m_vertexBuffer; };
  [[nodiscard]] IndexBuffer<uint32_t> *GetIndexBuffer() {
    return m_indexBuffer;
  };

  // DELETE COPY CONSTRUCTOR AND ASSIGNMENT
  // Because this class deletes vbo, ibo, and mat data, disabling copy avoids
  // dangling pointers
  Submesh(const Submesh &) = delete;
  Submesh &operator=(const Submesh &) = delete;

  //
  // --- MOVE CONSTRUCTOR -----------------------------------------------------
  Submesh(Submesh &&other) noexcept
      : m_parentMesh(other.m_parentMesh), m_vertexBuffer(other.m_vertexBuffer),
        m_indexBuffer(other.m_indexBuffer), m_material(other.m_material),
        m_id(other.m_id) {
    other.m_parentMesh = nullptr;
    other.m_vertexBuffer = nullptr;
    other.m_indexBuffer = nullptr;
    other.m_material = nullptr;
  }

  //
  // --- MOVE ASSIGNMENT ------------------------------------------------------
  Submesh &operator=(Submesh &&other) noexcept {
    if (this != &other) {
      Cleanup();

      m_parentMesh = other.m_parentMesh;
      m_vertexBuffer = other.m_vertexBuffer;
      m_indexBuffer = other.m_indexBuffer;
      m_material = other.m_material;
      m_id = other.m_id;

      other.m_parentMesh = nullptr;
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

  Mesh *m_parentMesh = nullptr;
  VertexBuffer *m_vertexBuffer = nullptr;
  IndexBuffer<uint32_t> *m_indexBuffer = nullptr;
  Material<T> *m_material = nullptr;
  uint32_t m_id = 0;
};

//
//
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// --- MESH -------------------------------------------------------------------
//
class Mesh : public GameObject {
public:
  // TODO: Handle the case of duplicating meshes & materials
  // TODO: Make m_indexBuffer a variant
  Mesh() { PC_PRINT("CREATED", TagType::Constr, "MESH"); };
  virtual ~Mesh() { PC_PRINT("DESTROYED", TagType::Destr, "MESH"); };

  virtual constexpr GameObjectTypes GetType() const override {
    return GameObjectTypes::Mesh;
  };

  // [[nodiscard]] inline UniformDefs::SubmeshUniform &GetMeshUniforms() {
  //   return m_uniform;
  // };

  template <MaterialTypes T>
  void AddSubmesh(VertexBuffer *vbo, IndexBuffer<uint32_t> *ibo,
                  Material<T> *mat) {
    PC_ASSERT(vbo && ibo && mat, "Submesh parameter(s) nullptr");

    if constexpr (T == MaterialTypes::BasicMat) {
      for (auto &submesh : m_basicSubmeshes) {
        if (submesh.GetId() == PC_GetHashedSubmeshId(vbo, ibo, mat)) {
          PC_WARN("Identical submesh already exists in Mesh")
          return;
        };
      }

      m_basicSubmeshes.emplace_back(this, vbo, ibo, mat);
    } else if constexpr (T == MaterialTypes::PbrMat) {
      for (auto &submesh : m_pbrSubmeshes) {
        if (submesh.GetId() == PC_GetHashedSubmeshId(vbo, ibo, mat)) {
          PC_WARN("Identical submesh already exists in Mesh")
          return;
        };
      }

      m_pbrSubmeshes.emplace_back(this, vbo, ibo, mat);
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
  std::vector<Submesh<MaterialTypes::BasicMat>> m_basicSubmeshes;
  std::vector<Submesh<MaterialTypes::PbrMat>> m_pbrSubmeshes;
};

//
//
//
//
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// --- UTIL FUNCTIONS (GLOBAL) ------------------------------------------------
template <MaterialTypes T>
bool PC_ValidateAndAddSubmesh(Submesh<T> *submesh,
                              std::vector<Submesh<T> *> &submeshes) {
  auto ptr = std::find(submeshes.begin(), submeshes.end(), submesh);

  if (ptr != submeshes.end()) {
    PC_WARN("Submesh already exists in the submesh library!")
    return false;
  };

  submeshes.emplace_back(submesh);
  return true;
};

template <MaterialTypes T>
bool PC_ValidateAndRemoveSubmesh(Submesh<T> *submesh,
                                 std::vector<Submesh<T> *> &submeshes) {
  auto ptr = std::find(submeshes.begin(), submeshes.end(), submesh);

  if (ptr == submeshes.end()) {
    PC_WARN("Submesh not found!")
    return false;
  };

  submeshes.erase(ptr);
  return true;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
