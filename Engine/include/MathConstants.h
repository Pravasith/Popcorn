#pragma once

#include "GlobalMacros.h"
#include <glm/ext/matrix_float4x4.hpp>

ENGINE_NAMESPACE_BEGIN

constexpr glm::mat4 PC_IDENTITY_MAT4 = glm::mat4(1.f);
constexpr glm::vec3 PC_ZERO_VEC3 = glm::vec3(0.f);

// Epsilon for error-free float comparisions
constexpr float PC_EPS = 1e-6f;

ENGINE_NAMESPACE_END
