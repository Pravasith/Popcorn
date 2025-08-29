#pragma once

#include "Animation.h"
#include "BufferObjects.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "Light.h"
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
  struct AnimTrackBounds {
    uint32_t startTimeS;
    uint32_t endTimeS;
  };

public:
  // File -> tinygltf Model
  static bool LoadFromFile(const std::string &filename, tinygltf::Model &model);

  // tinygltf Model -> GameObjects
  static void ExtractModelData(const tinygltf::Model &model,
                               std::vector<GameObject *> &gameObjects);

  static void ExtractAnimationsToAnimationTracks(
      const tinygltf::Model &model,
      std::vector<AnimationTrack> &animationTracks,
      std::vector<AnimTrackBounds> &bounds);

private:
  GltfLoader() = delete;
  ~GltfLoader() = delete;
  GltfLoader(const GltfLoader &) = delete;
  GltfLoader &operator=(const GltfLoader &) = delete;
  GltfLoader(GltfLoader &&) = delete;
  GltfLoader &operator=(GltfLoader &&) = delete;

  //
  // --- UTILS -----------------------------------------------------------------
  static GameObject *ConvertGltfNodeToGameObject(const tinygltf::Model &model,
                                                 int nodeIndex);
  static GameObject *CreateGameObjectByType(const tinygltf::Model &model,
                                            const tinygltf::Node &node);
  static void SetTransformData(const tinygltf::Node &node,
                               GameObject &gameObject);

  //
  // --- MESH UTILS ------------------------------------------------------------
  static void ExtractMeshData(const tinygltf::Model &model,
                              const tinygltf::Node &gltfNode, Mesh &mesh);
  static void ExtractLightsData(const tinygltf::Model &model,
                                const tinygltf::Node &node, Light *light);

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

  //
  // --- ANIMATION UTILS -------------------------------------------------------

private:
  static std::vector<GameObject *> s_nodeIndexToGameObjectPtrs;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
