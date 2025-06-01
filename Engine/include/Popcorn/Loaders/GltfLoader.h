#pragma once

#include "BufferObjects.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Material.h"
#include "Mesh.h"
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <tiny_gltf.h>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GltfLoader {
public:
  // File -> tinygltf Model
  static bool LoadFromFile(const std::string &filename, tinygltf::Model &model);

  // tinygltf Model -> GameObjects
  static void ExtractModelData(const tinygltf::Model &model,
                               std::vector<GameObject *> &gameObjects);

private:
  GltfLoader() { PC_PRINT("CREATED", TagType::Constr, "GltfLoader.h"); };
  ~GltfLoader() { PC_PRINT("DESTROYED", TagType::Destr, "GltfLoader.h"); };

  //
  // --- UTILS -----------------------------------------------------------------
  static GameObject *
  ConvertGltfNodeToGameObject(const tinygltf::Model &model,
                              const tinygltf::Node &gltfNode);
  static GameObject *CreateGameObjectByType(const tinygltf::Model &model,
                                            const tinygltf::Node &node);
  static void SetTransformData(const tinygltf::Node &node,
                               GameObject &gameObject);

  //
  // --- MESH UTILS ------------------------------------------------------------
  static void ExtractMeshData(const tinygltf::Model &model,
                              const tinygltf::Node &gltfNode, Mesh &mesh);
  [[nodiscard]] static VertexBuffer *
  ExtractVertexBuffer(const tinygltf::Model &model,
                      const tinygltf::Primitive &primitive);
  [[nodiscard]] static IndexBuffer<uint32_t> *
  ExtractIndexBuffer(const tinygltf::Model &model,
                     const tinygltf::Primitive &primitive);

  //
  // --- MATERIAL UTILS --------------------------------------------------------
  template <MaterialTypes T>
  [[nodiscard]] static DeriveMaterialDataType<T>::type
  ExtractMaterialData(const tinygltf::Model &model,
                      const tinygltf::Material &material);

  [[nodiscard]] static MaterialTypes
  GetGltfMaterialType(const tinygltf::Material &material);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
