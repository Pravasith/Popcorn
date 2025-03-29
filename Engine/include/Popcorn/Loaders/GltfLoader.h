#pragma once

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

struct GltfMesh {
  VertexBuffer *vertices;
  IndexBuffer<uint16_t> *indices;
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  GltfMaterial *material;
};

class GltfLoader {
public:
  static bool LoadFromFile(const std::string &filename, tinygltf::Model &model);
  static std::vector<GltfMesh> ExtractGltfMeshes(const tinygltf::Model &model);
  static void ProcessGLTFNode(const tinygltf::Model &model,
                              const tinygltf::Node &node,
                              const glm::mat4 &parentMatrix,
                              std::vector<GltfMesh> &meshes);

  static void ExtractVertexData(const tinygltf::Model &model,
                                const tinygltf::Primitive &primitive,
                                GltfMesh &outMesh);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
