#pragma once

#include "GlobalMacros.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct GltfMaterial {
  glm::vec4 baseColorFactor = glm::vec4(1.0f);
  float metallicFactor = 1.0f;
  float roughnessFactor = 1.0f;
  float alphaCutoff = 0.5f;
  bool doubleSided = false;
  bool hasBaseColorTexture = false;
  bool hasNormalTexture = false;
  bool hasMetallicRoughnessTexture = false;
} typedef GltfMaterialType;

struct GltfMesh {
  std::vector<float> vertices;
  std::vector<float> indices;
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  GltfMaterialType material;
} typedef GltfMeshType;

class GltfLoader {
public:
  static bool LoadFromFile(const std::string &filename, tinygltf::Model &model);
  static std::vector<GltfMeshType>
  ExtractGltfMeshes(const tinygltf::Model &model);
  static void ProcessGLTFNode(const tinygltf::Model &model,
                              const tinygltf::Node &node,
                              const glm::mat4 &parentMatrix,
                              std::vector<GltfMeshType> &meshes);

  static void ExtractVertexData(const tinygltf::Model &model,
                                const tinygltf::Primitive &primitive,
                                GltfMeshType &outMesh);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
