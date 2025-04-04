#pragma once

#include "Mesh.h"
#include <cstdint>
#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include "BufferObjects.h"
#include "GlobalMacros.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <tiny_gltf.h>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct GltfMaterial {
  bool doubleSided = false;
  bool hasBaseColorTexture = false;
  bool hasNormalTexture = false;
  bool hasMetallicRoughnessTexture = false;
  float metallicFactor = 1.0f;
  float roughnessFactor = 1.0f;
  float alphaCutoff = 0.5f;
  glm::vec4 baseColorFactor = glm::vec4(1.0f);
};

class GltfLoader {
public:
  // File -> tinygltf Model
  static bool LoadFromFile(const std::string &filename, tinygltf::Model &model);

  // tinygltf Model -> Meshes (vector)
  [[nodiscard]] static std::vector<Mesh *>
  ExtractGltfMeshes(const tinygltf::Model &model);

private:
  //
  // --- UTILS -----------------------------------------------------------------

  //
  // Converts tinygltf meshes to Mesh* vector
  static void ProcessGLTFNode(const tinygltf::Model &model,
                              const tinygltf::Node &node,
                              const glm::mat4 &parentMatrix,
                              std::vector<Mesh *> &meshes);
  [[nodiscard]] static VertexBuffer *
  ExtractVertexBuffer(const tinygltf::Model &model,
                      const tinygltf::Primitive &primitive);
  [[nodiscard]] static IndexBuffer<uint16_t> *
  ExtractIndexBuffer(const tinygltf::Model &model,
                     const tinygltf::Primitive &primitive);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
