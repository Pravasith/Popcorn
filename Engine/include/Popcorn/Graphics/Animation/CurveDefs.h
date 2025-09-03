#pragma once

#include "GlobalMacros.h"
#include <concepts>
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <typename U>
concept CurveValueType =
    std::same_as<U, float> || std::same_as<U, glm::vec2> ||
    std::same_as<U, glm::vec3> || std::same_as<U, glm::vec4>;

template <CurveValueType T> struct CurveInfoLinearForm {
  T p0;
  T p1;
};

template <CurveValueType T> struct CurveInfoBezierForm {
  T b0;
  T b1;
  T b2;
  T b3;
};

template <CurveValueType T> struct CurveInfoHermiteForm {
  T p0;
  T v0;
  T v1;
  T p1;
};

using CurveHashType = uint64_t;
using SplineHashType = uint64_t;

GFX_NAMESPACE_END ENGINE_NAMESPACE_END
