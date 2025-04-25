#pragma once

#include "GlobalMacros.h"
#include <glm/ext/matrix_float4x4.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
UNIFORM_DEFS_NAMESPACE_BEGIN

struct CameraUniform {
  glm::mat4 viewMatrix{1.0}; // aligned to 64 (16 * 4)
  glm::mat4 projMatrix{1.0}; // aligned to 64 (16 * 4)
};

struct GameObjectUniform {
  glm::mat4 worldMatrix{1.0}; // aligned to 64 (16 * 4);
};

struct BasicMaterialUniform {};

struct PbrMaterialUniform {};

UNIFORM_DEFS_NAMESPACE_END
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
