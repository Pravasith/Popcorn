#pragma once

#include "Animation.h"
#include "AnimationProperty.h"
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
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <type_traits>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct TimeTrain_Binding {
  TimeTrain tt;
  size_t animationTrackIndex;
};

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

// template <int T>
// using CurveValType = typename DimensionType<T>::type;

// Use for Quaternion to Euler conversions - when you want to pass a vec3
// AnimationProperty, and the outputData(from Blender) is Quaternion data(vec4),
// set QuatToEuler flag
template <int T, bool QuatToEuler>
using CurveValType =
    std::conditional_t<T == 4 && QuatToEuler, typename DimensionType<3>::type,
                       typename DimensionType<T>::type>::type;

template <int Dims, bool QuatToEuler = false>
// [[nodiscard]]
static inline const Spline<CurveValType<Dims, QuatToEuler>> *
PC_HermiteKnotToSpline_Helper(
    AnimationProperty<CurveValType<Dims, QuatToEuler>> *animationPropertyPtr,
    size_t outputAccessorCount, const float *inputData,
    const float
        *outputTripletsData // Hermite output has 3 components - vIn, val, vOut
) {
  static_assert(Dims >= 1 && Dims <= 4);
  using CurveType = DimensionType<Dims>::type;
  std::vector<HermiteKnot<CurveType>> hermiteKnots;
  const Spline<CurveValType<Dims, QuatToEuler>> *splinePtr;

  size_t keyframeCount = outputAccessorCount /
                         3; // Hermite output has 3 components - vIn, val, vOut

  assert(inputData[keyframeCount - 1] - inputData[0] <= 1.0 &&
         "Length (or journey time of time train after conversion) of "
         "individual 'Blender Actions' cannot be more than 1");

  hermiteKnots.reserve(keyframeCount);

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

  // TODO: Convert quats to Eulers
  if constexpr (Dims == 4 && QuatToEuler) {
    std::vector<HermiteKnot<glm::vec3>> eulerHermiteKnots;

    for (HermiteKnot<glm::vec4> &knot : hermiteKnots) {
      HermiteKnot<glm::vec3> eulerHermiteKnot;

      glm::quat quatHermiteKnot_vIn(knot.vIn[3], knot.vIn[0], knot.vIn[1],
                                    knot.vIn[2]);
      glm::quat quatHermiteKnot_val(knot.val[3], knot.val[0], knot.val[1],
                                    knot.val[2]);
      glm::quat quatHermiteKnot_vOut(knot.vOut[3], knot.vOut[0], knot.vOut[1],
                                     knot.vOut[2]);

      eulerHermiteKnot.vIn = glm::eulerAngles(quatHermiteKnot_vIn);
      eulerHermiteKnot.val = glm::eulerAngles(quatHermiteKnot_val);
      eulerHermiteKnot.vOut = glm::eulerAngles(quatHermiteKnot_vOut);
      eulerHermiteKnot.t = knot.t;

      eulerHermiteKnots.push_back(eulerHermiteKnot);
    }

    splinePtr = SplineFactory::Get()->MakeSpline(eulerHermiteKnots);
  } else {
    splinePtr = SplineFactory::Get()->MakeSpline(hermiteKnots);
  }

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

  TimeTrain_Binding ttBinding;
  ttBinding.animationTrackIndex = bT_flr / 2;
  // ttBinding.tt = TimeTrain(

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
