#pragma once

#include "Animation.h"
#include "GlobalMacros.h"
#include "SplineDefs.h"
#include "SplineFactory.h"
#include "Splines.h"
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
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

  size_t keyframeCount = outputAccessorCount / 3;

  assert(inputData[keyframeCount - 1] - inputData[0] <= 1.0 &&
         "Length (or journey time of time train after conversion) of "
         "individual 'Blender Actions' cannot be more than 1");

  hermiteKnots.reserve(
      keyframeCount); // keyframe count = outputTripletsData / 3

  // outputAccessorCount is 3 * keyframeCount
  for (size_t i = 0; i < outputAccessorCount; i += 3) {
    CurveType vIn{};
    CurveType val{};
    CurveType vOut{};

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

  const Spline<typename DimensionType<Dims>::type> *splinePtr =
      SplineFactory::Get()->MakeSpline(hermiteKnots);

  float boardTime = inputData[0];                // unnormalized
  float destTime = inputData[keyframeCount - 1]; // unnormalized

  uint32_t bT_flr = (uint32_t)floorf(boardTime);
  uint32_t dT_ceil = (uint32_t)ceilf(destTime);

  // n in terms of board & dest
  // n = (dest - 1) / 2
  // n = board / 2
  assert(((dT_ceil - 1) / 2) == (bT_flr / 2) &&
         "Board & dest times should belong to ranges 2n, 2n + 1 respectively, "
         "i.e. in (0,1), (2,3), (4,5), ... (2n, 2n+1)");

  float bT_norm = boardTime - bT_flr; // use this in time trains
  float dT_norm = destTime - bT_flr;  // use this in time trains

  struct TimeTrainInSlot {
    TimeTrain tt;
    size_t animationTrackSlot;
  };

  TimeTrainInSlot ttSlot;
  ttSlot.animationTrackSlot = bT_flr / 2;

  // Conversion
  //
  // 3.5 -> 3.8
  //  .5 ->  .8
  //

  return splinePtr;
}

template <int Dims>
// [[nodiscard]]
static inline const Spline<typename DimensionType<Dims>::type> *
PC_LinearKnotToSpline_Helper(size_t outputAccessorCount, const float *inputData,
                             const float *outputData) {
  static_assert(Dims >= 1 && Dims <= 4);
  using CurveType = DimensionType<Dims>::type;
  std::vector<LinearKnot<CurveType>> linearKnots;
  linearKnots.reserve(outputAccessorCount);

  for (size_t i = 0; i < outputAccessorCount; ++i) {
    CurveType val{};

    for (size_t j = 0; j < Dims; ++j) {
      // Fill individual vectors (or floats), e.g. vec3[0], vec3[1], vec3[2]
      if constexpr (Dims == 1) {
        val = outputData[i];
      } else {
        val[j] = outputData[i * Dims + j];
      }
    }
    linearKnots.emplace_back(val, inputData[i]);
  }
  return SplineFactory::Get()->MakeSpline(linearKnots);
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
