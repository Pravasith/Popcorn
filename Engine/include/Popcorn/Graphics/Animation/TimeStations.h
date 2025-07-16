#pragma once

#include "Animation/AnimationDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <unordered_map>
#include <variant>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

using CurveType = std::variant<Curve<float>, Curve<glm::vec2>, Curve<glm::vec3>,
                               Curve<glm::vec4>>;
using CurvePtrType =
    const std::variant<const Curve<float> *, const Curve<glm::vec2> *,
                       const Curve<glm::vec3> *, const Curve<glm::vec4> *>;

class CurveFactory {
public:
  CurveFactory() = default;
  ~CurveFactory() {
    // TODO: Delete curves in the library (static alloc)
  };

  // TODO: Fill out
  template <CurveForm T, CurveValueType P>
  static void MakeCurve(DeriveCurveInfoType<T, P> curveInfo) {
    if constexpr (T == CurveForm::Linear) {
      CurveInfoLinearForm<P> &info = curveInfo;
      LinearCurve<P> curve(curveInfo);

    } else if constexpr (T == CurveForm::Bezier) {
      CurveInfoBezierForm<P> &info = curveInfo;
      BezierCurve<P> curve(curveInfo);

    } else if constexpr (T == CurveForm::Hermite) {
      CurveInfoHermiteForm<P> &info = curveInfo;
    }
  };

  // TODO: Fill out
  // template <CurveForm T, ValidCurveParamType P>
  // static void MakeSplineFromCurves() {
  //   if constexpr (T == CurveForm::Linear) {
  //     CurveInfoLinearForm<P> &info = curveInfo;
  //   } else if constexpr (T == CurveForm::Bezier) {
  //     CurveInfoBezierForm<P> &info = curveInfo;
  //   } else if constexpr (T == CurveForm::Hermite) {
  //     CurveInfoHermiteForm<P> &info = curveInfo;
  //   }
  // };

private:
  static std::unordered_map<const std::string, CurveType> s_curveLibrary;
};

class TimeRail {
  TimeRail(const CurvePtrType curvePtr)
      : m_curvePtr(curvePtr) {

        };

  const CurvePtrType m_curvePtr;
  float connectingStations[2]{
      0.0f, 1.0f}; // Array value can be anything [0, 1].
                   // Relative to the time in the AnimationTrack
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
