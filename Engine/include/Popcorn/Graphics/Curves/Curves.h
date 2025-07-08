#pragma once

#include "CurvesDefs.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <string>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <CurveType T, ValidCurveParamType ParamType> class Curve {
public:
  Curve() { PC_PRINT("CREATED", TagType::Constr, "Curve") };
  ~Curve() { PC_PRINT("DESTROYED", TagType::Destr, "Curve") };

  float GetValueAt_Fast(float t) {
    if constexpr (T == CurveType::Linear) {
    } else if constexpr (T == CurveType::Bezier) {
    } else if constexpr (T == CurveType::Hermite) {
    }
  };
  double GetValueAt_Slow(float t) {
    if constexpr (T == CurveType::Linear) {
      LinearInfo<ParamType> &curveInfo = m_curveInfo;
      return (1 - t) * curveInfo.p0 + t * curveInfo.p1;
    } else if constexpr (T == CurveType::Bezier) {
    } else if constexpr (T == CurveType::Hermite) {
    }
  };

protected:
  typename DeriveCurveInfoType<T, ParamType>::type m_curveInfo;
};

template <SplineType V, ValidCurveParamType ParamType> class Spline {
public:
  static constexpr SplineType typeValue = V;

public:
  Spline() { PC_PRINT("CREATED", TagType::Constr, "Spline") };
  ~Spline() { PC_PRINT("DESTROYED", TagType::Destr, "Spline") };

protected:
  float m_globalT = 0.0f; // 'the' parameter. t as in p(t)

  // TODO: Change CurveType based on SplineType
  std::vector<Curve<CurveType::Bezier, ParamType> *> m_knots{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
