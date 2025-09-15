#pragma once

#include "Animation.h"
#include "AnimationProperty.h"
#include "CurveDefs.h"
#include "CurveFactory.h"
#include "GlobalMacros.h"
#include "SplineDefs.h"
#include "SplineFactory.h"
#include "Splines.h"
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <glm/ext/quaternion_geometric.hpp>
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

//
//
//
//
// --- HERMITE DATA PROCESSING ---------------------------------------------
// --- HERMITE DATA PROCESSING ---------------------------------------------
//

// template <int T>
// using CurveValType = typename DimensionType<T>::type;

template <int T, bool IsQuat>
using CurveValType = std::conditional_t<T == 4 && IsQuat, glm::quat,
                                        typename DimensionType<T>::type>;

template <int Dims, bool IsQuat = false>
[[nodiscard]] static inline TimeTrain_Binding
PC_CreateTimeTrainBindingFromGltfActions_HermiteData(
    AnimationProperty<CurveValType<Dims, IsQuat>> *animationPropertyPtr,
    size_t outputAccessorCount, const float *inputData,
    const float
        *outputTripletsData // Hermite output has 3 components - vIn, val, vOut
) {
  static_assert(Dims >= 1 && Dims <= 4);
  using CurveType = DimensionType<Dims>::type;
  std::vector<HermiteKnot<CurveType>> hermiteKnots;

  size_t keyframeCount = outputAccessorCount /
                         3; // Hermite output has 3 components - vIn, val, vOut

  const Spline<CurveValType<Dims, IsQuat>> *splinePtr = nullptr;
  const Curve<CurveValType<Dims, IsQuat>> *curvePtr = nullptr;

  bool isRailSpline = true;

  if (keyframeCount == 2) {
    isRailSpline = false;
  }

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

  if constexpr (Dims == 4 && IsQuat) {
    std::vector<HermiteKnot<glm::quat>> quatHermiteKnots;

    // Just swizzling to convert gltf export -> glm format
    for (HermiteKnot<glm::vec4> &knot : hermiteKnots) {
      HermiteKnot<glm::quat> quatHermiteKnot;

      glm::quat quatHermiteKnot_vIn(knot.vIn[3], knot.vIn[0], knot.vIn[1],
                                    knot.vIn[2]);
      glm::quat quatHermiteKnot_val(knot.val[3], knot.val[0], knot.val[1],
                                    knot.val[2]);
      glm::quat quatHermiteKnot_vOut(knot.vOut[3], knot.vOut[0], knot.vOut[1],
                                     knot.vOut[2]);

      quatHermiteKnot.vIn = quatHermiteKnot_vIn;
      quatHermiteKnot.val = glm::normalize(quatHermiteKnot_val);
      quatHermiteKnot.vOut = quatHermiteKnot_vOut;
      quatHermiteKnot.t = knot.t;

      quatHermiteKnots.push_back(quatHermiteKnot);
    }

    // Check if all knots are in the same "hemisphere" of the 4D Hypersphere, if
    // not, we gotta bring the ones in the other hemisphere to the same
    // hemisphere
    for (size_t i = 1; i < quatHermiteKnots.size(); ++i) {
      if (glm::dot(quatHermiteKnots[i - 1].val, quatHermiteKnots[i].val) <
          0.0f) {
        quatHermiteKnots[i].vIn = -quatHermiteKnots[i].vIn;
        quatHermiteKnots[i].val = -quatHermiteKnots[i].val;
        quatHermiteKnots[i].vOut = -quatHermiteKnots[i].vOut;
      }
    }

    if (isRailSpline) {
      splinePtr = SplineFactory::Get()->MakeSpline(quatHermiteKnots);
    } else {
      assert(quatHermiteKnots.size() == 2);

      CurveInfoHermiteForm<glm::quat> cInfo;
      cInfo.p0 = quatHermiteKnots[0].val;
      cInfo.v0 = quatHermiteKnots[0].vOut;
      cInfo.v1 = quatHermiteKnots[1].vIn;
      cInfo.p1 = quatHermiteKnots[1].val;

      curvePtr = CurveFactory::Get()->GetCurvePtr(cInfo);
    }
  } else {
    // Non Quaternions
    if (isRailSpline) {
      splinePtr = SplineFactory::Get()->MakeSpline(hermiteKnots);
    } else {
      assert(hermiteKnots.size() == 2);

      CurveInfoHermiteForm<CurveType> cInfo;
      cInfo.p0 = hermiteKnots[0].val;
      cInfo.v0 = hermiteKnots[0].vOut;
      cInfo.v1 = hermiteKnots[1].vIn;
      cInfo.p1 = hermiteKnots[1].val;

      curvePtr = CurveFactory::Get()->GetCurvePtr(cInfo);
    }
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

  TimeTrain tt =
      isRailSpline
          ? TimeTrain(animationPropertyPtr, splinePtr, bT_norm, dT_norm)
          : TimeTrain(animationPropertyPtr, curvePtr, bT_norm, dT_norm);

  TimeTrain_Binding ttBinding{};
  ttBinding.animationTrackIndex = bT_flr / 2;
  ttBinding.tt = tt;

  return ttBinding;
}

//
//
//
//
// --- LINEAR DATA PROCESSING ----------------------------------------------
// --- LINEAR DATA PROCESSING ----------------------------------------------
//

template <int Dims, bool IsQuat = false>
[[nodiscard]] static inline TimeTrain_Binding
PC_CreateTimeTrainBindingFromGltfActions_LinearData(
    AnimationProperty<CurveValType<Dims, IsQuat>> *animationPropertyPtr,
    size_t outputAccessorCount, const float *inputData,
    const float *outputData // Linear output has 1 component - val
) {

  static_assert(Dims >= 1 && Dims <= 4);
  using CurveType = DimensionType<Dims>::type;
  std::vector<LinearKnot<CurveType>> linearKnots;

  size_t keyframeCount =
      outputAccessorCount / 1; // Linear output has 1 components - val

  const Spline<CurveValType<Dims, IsQuat>> *splinePtr = nullptr;
  const Curve<CurveValType<Dims, IsQuat>> *curvePtr = nullptr;

  bool isRailSpline = true;

  if (keyframeCount == 2) {
    isRailSpline = false;
  }

  assert(inputData[keyframeCount - 1] - inputData[0] <= 1.0 &&
         "Length (or journey time of time train after conversion) of "
         "individual 'Blender Actions' cannot be more than 1");

  linearKnots.reserve(keyframeCount);

  for (size_t i = 0; i < outputAccessorCount; ++i) {
    CurveType val{};
    size_t val_idx = i;

    for (size_t j = 0; j < Dims; ++j) {
      // Fill individual vectors (or floats), e.g. vec3[0], vec3[1], vec3[2]
      if constexpr (Dims == 1) {
        val = outputData[val_idx];
      } else {
        val[j] = outputData[val_idx * Dims + j];
      }
    }

    linearKnots.emplace_back(val, inputData[i]);
  }

  if constexpr (Dims == 4 && IsQuat) {
    std::vector<LinearKnot<glm::quat>> quatLinearKnots;

    for (LinearKnot<glm::vec4> &knot : linearKnots) {
      LinearKnot<glm::quat> quatLinearKnot;

      glm::quat quatLinearKnot_val(knot.val[3], knot.val[0], knot.val[1],
                                   knot.val[2]);

      quatLinearKnot.val = glm::normalize(quatLinearKnot_val);
      quatLinearKnot.t = knot.t;

      quatLinearKnots.push_back(quatLinearKnot);
    }

    // Check if all knots are in the same "hemisphere" of the 4D Hypersphere, if
    // not, we gotta bring the ones in the other hemisphere to the same
    // hemisphere
    for (size_t i = 1; i < quatLinearKnots.size(); ++i) {
      if (glm::dot(quatLinearKnots[i - 1].val, quatLinearKnots[i].val) < 0.0f) {
        quatLinearKnots[i].val = -quatLinearKnots[i].val;
      }
    }

    if (isRailSpline) {
      splinePtr = SplineFactory::Get()->MakeSpline(quatLinearKnots);
    } else {
      assert(quatLinearKnots.size() == 2);

      CurveInfoLinearForm<glm::quat> cInfo;
      cInfo.p0 = quatLinearKnots[0].val;
      cInfo.p1 = quatLinearKnots[1].val;

      curvePtr = CurveFactory::Get()->GetCurvePtr(cInfo);
    }
  } else {
    if (isRailSpline) {
      splinePtr = SplineFactory::Get()->MakeSpline(linearKnots);
    } else {
      assert(linearKnots.size() == 2);

      CurveInfoLinearForm<CurveType> cInfo;
      cInfo.p0 = linearKnots[0].val;
      cInfo.p1 = linearKnots[1].val;

      curvePtr = CurveFactory::Get()->GetCurvePtr(cInfo);
    }
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

  TimeTrain tt =
      isRailSpline
          ? TimeTrain(animationPropertyPtr, splinePtr, bT_norm, dT_norm)
          : TimeTrain(animationPropertyPtr, curvePtr, bT_norm, dT_norm);

  TimeTrain_Binding ttBinding{};
  ttBinding.animationTrackIndex = bT_flr / 2;
  ttBinding.tt = tt;

  return ttBinding;
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
