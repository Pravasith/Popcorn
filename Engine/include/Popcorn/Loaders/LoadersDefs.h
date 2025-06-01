#pragma once

#include "BufferObjects.h"
#include "GlobalMacros.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

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

inline static BufferDefs::Layout GetGltfVertexBufferLayout() {
  using namespace BufferDefs;

  BufferDefs::Layout layout;
  layout.Set<AttrTypes::Float3, AttrTypes::Float3, AttrTypes::Float2>();
  return layout;
}

inline const BufferDefs::Layout GltfVertexBufferLayout =
    GetGltfVertexBufferLayout();

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
