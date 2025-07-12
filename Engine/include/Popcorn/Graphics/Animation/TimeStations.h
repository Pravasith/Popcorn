#pragma once

#include "Curves.h"
#include "GlobalMacros.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <variant>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

using CurvePtrType =
    const std::variant<const Curve<float> *, const Curve<double> *,
                       const Curve<glm::vec3> *, const Curve<glm::vec2> *,
                       const Curve<glm::mat4> *
                       // TODO: Add more as needed
                       >;

struct TimeRail {
  CurvePtrType curve;
  float connectingStations[2]{
      0.0f, 1.0f}; // Array value can be anything [0, 1].
                   // Relative to the time in the AnimationTrack
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
