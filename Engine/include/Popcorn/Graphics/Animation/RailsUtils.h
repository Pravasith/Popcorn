#pragma once

#include "CurveDefs.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "SplineDefs.h"
#include <cassert>
#include <cstdint>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <CurveValueType T>
inline static CurveHashType
PC_HashCurveInfo_Linear(const CurveInfoLinearForm<T> &curveInfo) {
  uint64_t hash = 0;

  if constexpr (std::is_same_v<T, float>) {
    hash ^= (PC_FloatToUint64(curveInfo.p0) << 2);
    hash ^= (PC_FloatToUint64(curveInfo.p1) >> 3);
  } else if constexpr (std::is_same_v<T, glm::vec2>) {
    for (int i = 0; i < 2; ++i) {
      hash ^= (PC_FloatToUint64(curveInfo.p0[i]) << (i * 2));
      hash ^= (PC_FloatToUint64(curveInfo.p1[i]) >> (i * 3));
    }
  } else if constexpr (std::is_same_v<T, glm::vec3>) {
    for (int i = 0; i < 3; ++i) {
      hash ^= (PC_FloatToUint64(curveInfo.p0[i]) << (i * 2));
      hash ^= (PC_FloatToUint64(curveInfo.p1[i]) >> (i * 3));
    }
  } else if constexpr (std::is_same_v<T, glm::vec4>) {
    for (int i = 0; i < 4; ++i) {
      hash ^= (PC_FloatToUint64(curveInfo.p0[i]) << (i * 2));
      hash ^= (PC_FloatToUint64(curveInfo.p1[i]) >> (i * 3));
    }
  }

  return hash * 2654435761ull;
}

template <CurveValueType T>
inline static CurveHashType
PC_HashCurveInfo_Bezier(const CurveInfoBezierForm<T> &curveInfo) {
  uint64_t hash = 0;

  if constexpr (std::is_same_v<T, float>) {
    hash ^= (PC_FloatToUint64(curveInfo.b0) << 2);
    hash ^= (PC_FloatToUint64(curveInfo.b1) >> 3);
    hash ^= (PC_FloatToUint64(curveInfo.b2) << 1);
    hash ^= (PC_FloatToUint64(curveInfo.b3) >> 1);
  } else if constexpr (std::is_same_v<T, glm::vec2>) {
    for (int i = 0; i < 2; ++i) {
      hash ^= (PC_FloatToUint64(curveInfo.b0[i]) << (i * 2));
      hash ^= (PC_FloatToUint64(curveInfo.b1[i]) >> (i * 3));
      hash ^= (PC_FloatToUint64(curveInfo.b2[i]) << i);
      hash ^= (PC_FloatToUint64(curveInfo.b3[i]) >> i);
    }
  } else if constexpr (std::is_same_v<T, glm::vec3>) {
    for (int i = 0; i < 3; ++i) {
      hash ^= (PC_FloatToUint64(curveInfo.b0[i]) << (i * 2));
      hash ^= (PC_FloatToUint64(curveInfo.b1[i]) >> (i * 3));
      hash ^= (PC_FloatToUint64(curveInfo.b2[i]) << i);
      hash ^= (PC_FloatToUint64(curveInfo.b3[i]) >> i);
    }
  } else if constexpr (std::is_same_v<T, glm::vec4>) {
    for (int i = 0; i < 4; ++i) {
      hash ^= (PC_FloatToUint64(curveInfo.b0[i]) << (i * 2));
      hash ^= (PC_FloatToUint64(curveInfo.b1[i]) >> (i * 3));
      hash ^= (PC_FloatToUint64(curveInfo.b2[i]) << i);
      hash ^= (PC_FloatToUint64(curveInfo.b3[i]) >> i);
    }
  }

  return hash * 2654435761ull;
}

template <CurveValueType T>
inline static CurveHashType
PC_HashCurveInfo_Hermite(const CurveInfoHermiteForm<T> &curveInfo) {
  uint64_t hash = 0;

  if constexpr (std::is_same_v<T, float>) {
    hash ^= (PC_FloatToUint64(curveInfo.p0) << 2);
    hash ^= (PC_FloatToUint64(curveInfo.v0) >> 3);
    hash ^= (PC_FloatToUint64(curveInfo.v1) << 1);
    hash ^= (PC_FloatToUint64(curveInfo.p1) >> 1);
  } else if constexpr (std::is_same_v<T, glm::vec2>) {
    for (int i = 0; i < 2; ++i) {
      hash ^= (PC_FloatToUint64(curveInfo.p0[i]) << (i * 2));
      hash ^= (PC_FloatToUint64(curveInfo.v0[i]) >> (i * 3));
      hash ^= (PC_FloatToUint64(curveInfo.v1[i]) << i);
      hash ^= (PC_FloatToUint64(curveInfo.p1[i]) >> i);
    }
  } else if constexpr (std::is_same_v<T, glm::vec3>) {
    for (int i = 0; i < 3; ++i) {
      hash ^= (PC_FloatToUint64(curveInfo.p0[i]) << (i * 2));
      hash ^= (PC_FloatToUint64(curveInfo.v0[i]) >> (i * 3));
      hash ^= (PC_FloatToUint64(curveInfo.v1[i]) << i);
      hash ^= (PC_FloatToUint64(curveInfo.p1[i]) >> i);
    }
  } else if constexpr (std::is_same_v<T, glm::vec4>) {
    for (int i = 0; i < 4; ++i) {
      hash ^= (PC_FloatToUint64(curveInfo.p0[i]) << (i * 2));
      hash ^= (PC_FloatToUint64(curveInfo.v0[i]) >> (i * 3));
      hash ^= (PC_FloatToUint64(curveInfo.v1[i]) << i);
      hash ^= (PC_FloatToUint64(curveInfo.p1[i]) >> i);
    }
  }

  return hash * 2654435761ull;
}

template <CurveValueType T>
inline static SplineHashType
PC_HashSplineSegment(const std::vector<SplineSegment<T>> &segments) {
  assert(!segments.empty());

  uint64_t hash = 0;

  for (const SplineSegment<T> &seg : segments) {
    if (seg.reparameterizationCurve) {
      hash ^= reinterpret_cast<uintptr_t>(seg.reparameterizationCurve) >> 3;
    }
    hash ^= reinterpret_cast<uintptr_t>(seg.curve) << 2;
    hash ^= PC_DoubleToUint64(seg.t) >> 3;
  }

  return hash * 2654435761ull;
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
