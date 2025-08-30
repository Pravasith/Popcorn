#pragma once

#include "GlobalMacros.h"
#include "SplineDefs.h"
#include "SplineFactory.h"
#include "Splines.h"
#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <int T> struct DimensionType;
template <> struct DimensionType<1> {
  using type = float;
};
template <> struct DimensionType<2> {
  using type = glm::vec2;
};
template <> struct DimensionType<3> {
  using type = glm::vec3;
};
template <> struct DimensionType<4> {
  using type = glm::vec4;
};

template <int Dims>
// [[nodiscard]]
static inline const Spline<typename DimensionType<Dims>::type> *
PC_HermiteKnotToSpline_Helper(
    size_t outputAccessorCount, const float *inputData,
    const float *outputTripletsData // triplets bc Blender exports Hermite
) {
  static_assert(Dims >= 1 && Dims <= 4);
  using CurveType = DimensionType<Dims>::type;
  std::vector<HermiteKnot<CurveType>> hermiteKnots;
  hermiteKnots.reserve(outputAccessorCount /
                       3); // keyframe count = outputTripletsData / 3

  // outputAccessorCount is 3 * keyframeCount
  for (size_t i = 0; i < outputAccessorCount; i += 3) {
    CurveType vIn;
    CurveType val;
    CurveType vOut;

    size_t vIn_idx = i + 0;
    size_t val_idx = i + 1;
    size_t vOut_idx = i + 2;

    for (size_t j = 0; j < Dims; ++j) {
      // Fill individual vectors (or floats), e.g. vec3[0], vec3[1], vec3[2]
      if constexpr (Dims == 1) {
        vIn = outputTripletsData[vIn_idx];
        val = outputTripletsData[val_idx];
        vOut = outputTripletsData[vOut_idx];
      } else {
        vIn[j] = outputTripletsData[vIn_idx * Dims + j];
        val[j] = outputTripletsData[val_idx * Dims + j];
        vOut[j] = outputTripletsData[vOut_idx * Dims + j];
      }
    }
    hermiteKnots.emplace_back(vIn, val, vOut, inputData[i / 3]);
  }
  return SplineFactory::Get()->MakeSpline(hermiteKnots);
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
