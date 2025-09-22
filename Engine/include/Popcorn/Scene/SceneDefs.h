#pragma once

#include "GlobalMacros.h"
#include <glm/ext/vector_float3.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// World up vector (if local Up vector is needed, then lookAt & orientation
// needs to be recomputed too)
static constexpr glm::vec3 PC_WORLD_UP_DIR{0.f, 1.f, 0.f};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
