#pragma once

#include "Animation.h"
#include "AnimationProperty.h"
#include "Base.h"
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

template <int T, bool IsQuat>
using CurveValType = std::conditional_t<T == 4 && IsQuat, glm::quat,
                                        typename DimensionType<T>::type>;

template <int Dims, bool IsQuat = false>
[[nodiscard]] static inline TimeTrain_Binding
PC_CreateTimeTrainBindingFromGltfActions_HermiteData(
    AnimationProperty<CurveValType<Dims, IsQuat>> *animationPropertyPtr,
    uint32_t beginIdx, uint32_t endIdx, const float *inputData,
    const float *outputData // Hermite output has 3 components -
                            // vIn, val, vOut
) {
  static_assert(Dims >= 1 && Dims <= 4);
  using CurveType = DimensionType<Dims>::type;
  std::vector<HermiteKnot<CurveType>> hermiteKnots;

  const Spline<CurveValType<Dims, IsQuat>> *splinePtr = nullptr;
  const Curve<CurveValType<Dims, IsQuat>> *curvePtr = nullptr;

  bool isRailSpline = true;
  uint32_t keyframeCount = endIdx - beginIdx;

  if (keyframeCount == 2) {
    isRailSpline = false;
  }

  assert(inputData[endIdx - 1] - inputData[beginIdx] <= 1.0 &&
         "Length (or journey time of time train after conversion) of "
         "individual 'Blender Actions' cannot be more than 1");

  hermiteKnots.reserve(keyframeCount);

  // outputAccessorCount is 3 * keyframeCount
  for (size_t i = beginIdx; i < endIdx; ++i) {
    CurveType vIn{};
    CurveType val{};
    CurveType vOut{};

    size_t vIn_idx = i * 3 + 0;
    size_t val_idx = i * 3 + 1;
    size_t vOut_idx = i * 3 + 2;

    for (size_t j = 0; j < Dims; ++j) {
      // Fill individual vectors (or floats), e.g. vec3[0], vec3[1], vec3[2]
      if constexpr (Dims == 1) {
        vIn = outputData[vIn_idx];
        val = outputData[val_idx];
        vOut = outputData[vOut_idx];
      } else {
        vIn[j] = outputData[vIn_idx * Dims + j];
        val[j] = outputData[val_idx * Dims + j];
        vOut[j] = outputData[vOut_idx * Dims + j];
      }
    }

    float inputDataNorm = inputData[i] - inputData[beginIdx];
    hermiteKnots.emplace_back(vIn, val, vOut, inputDataNorm);
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

  float boardTime = inputData[beginIdx];  // unnormalized
  float destTime = inputData[endIdx - 1]; // unnormalized

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
    uint32_t beginIdx, uint32_t endIdx, const float *inputData,
    const float *outputData // Linear output has 1 component - val
) {

  static_assert(Dims >= 1 && Dims <= 4);
  using CurveType = DimensionType<Dims>::type;
  std::vector<LinearKnot<CurveType>> linearKnots;

  const Spline<CurveValType<Dims, IsQuat>> *splinePtr = nullptr;
  const Curve<CurveValType<Dims, IsQuat>> *curvePtr = nullptr;

  bool isRailSpline = true;
  uint32_t keyframeCount = endIdx - beginIdx;

  if (keyframeCount == 2) {
    isRailSpline = false;
  }

  assert(inputData[endIdx - 1] - inputData[beginIdx] <= 1.0 &&
         "Length (or journey time of time train after conversion) of "
         "individual 'Blender Actions' cannot be more than 1");

  linearKnots.reserve(keyframeCount);

  for (size_t i = beginIdx; i < endIdx; ++i) {
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

    float inputDataNorm = inputData[i] - inputData[beginIdx];
    linearKnots.emplace_back(val, inputDataNorm);
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

  float boardTime = inputData[beginIdx];  // unnormalized
  float destTime = inputData[endIdx - 1]; // unnormalized

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
// --- MAKE TIME TRAIN BINDINGS --------------------------------------------
// --- MAKE TIME TRAIN BINDINGS --------------------------------------------
// --- MAKE TIME TRAIN BINDINGS --------------------------------------------
//

enum GltfKnotTypes { HermiteKnotType = 1, LinearKnotType, StepKnotType };

template <int Dims, bool IsQuat = false>
static void PC_MakeTimeTrainBindings(
    AnimationProperty<CurveValType<Dims, IsQuat>> *animationPropertyPtr,
    GltfKnotTypes knotType, size_t keyframeCount, const float *inputData,
    const float *outputData, // Linear output has 1 component - val
    std::vector<TimeTrain_Binding> &allTimeTrainBindings) {
  uint32_t beginIdx = 0;
  uint32_t endIdx = 0;
  uint32_t n = floorf(inputData[beginIdx]) / 2;

  // Example keyframe input data
  // 0, 0.3, 0.6. 1
  // 2, 2.3, 3
  // 2n, ..., 2n+1

  while (beginIdx < keyframeCount) {
    while (endIdx < keyframeCount && inputData[endIdx] >= 2 * n &&
           inputData[endIdx] <= 2 * n + 1) {
      ++endIdx;
    }

    // create the train binding from prev bytes (begin -> end(end is
    // excl.))
    if (endIdx - beginIdx > 1) {
      if (knotType == HermiteKnotType) {
        allTimeTrainBindings.emplace_back(
            PC_CreateTimeTrainBindingFromGltfActions_HermiteData<Dims, true>(
                animationPropertyPtr, beginIdx, endIdx, inputData,
                outputData)); // endIdx is excl.
      } else if (knotType == LinearKnotType) {
        allTimeTrainBindings.emplace_back(
            PC_CreateTimeTrainBindingFromGltfActions_LinearData<Dims, true>(
                animationPropertyPtr, beginIdx, endIdx, inputData,
                outputData)); // endIdx is excl.
      }
    } else {
      PC_WARN("One keyframe detected, skipping to the next section")
    }

    if (endIdx >= keyframeCount)
      break;

    beginIdx = endIdx;
    n = floorf(inputData[beginIdx]) / 2;
  }

  PC_PRINT("TEST N: " << n << "\nKeyframe count :" << keyframeCount,
           TagType::Print, "")
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
