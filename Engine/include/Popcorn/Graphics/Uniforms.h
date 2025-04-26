#pragma once

#include "GlobalMacros.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/fwd.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
UNIFORM_DEFS_NAMESPACE_BEGIN

struct CameraUniform {
  glm::mat4 viewMatrix{1.0f}; // aligned to 64 - each column-vector is 16
  glm::mat4 projMatrix{1.0f}; // aligned to 64 - each column-vector is 16
};

struct GameObjectUniform {
  glm::mat4 worldMatrix{1.0f}; // aligned to 64
};

struct BasicMaterialUniform {
  glm::vec4 baseColorFactor{1.0f}; // aligned to 16
};

struct PbrMaterialUniform {
  glm::vec4 baseColorFactor{1.0f};     // aligned to 16
  float metallicFactor = 1.0f;         // aligned to 4
  float roughnessFactor = 1.0f;        // aligned to 4
  float alphaCutoff = 0.5f;            // aligned to 4
  int hasBaseColorTexture = 0;         // aligned to 4
  int hasNormalTexture = 0;            // aligned to 4
  int hasMetallicRoughnessTexture = 0; // aligned to 4
};

UNIFORM_DEFS_NAMESPACE_END
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
