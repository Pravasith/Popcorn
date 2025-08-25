#pragma once

#include "CurveDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <CurveValueType T> struct HermiteKnot {
  T vIn;
  T val;
  T vOut;
  double t; // [0, 1)
};

template <CurveValueType T> struct LinearKnot {
  T val;
  double t; // [0, 1)
};

//
template <CurveValueType T>
using SplineInfo_BlenderAnimations_HermiteKnots = std::vector<HermiteKnot<T>>;
//

// For physical representation of curves (e.g. rails)
template <CurveValueType T> struct SplineInfo_BlenderJSONExport_BezierForm {
  std::vector<CurveInfoBezierForm<T>> bezierCurveInfoValues;
  std::vector<double> tValues; // 't' or 'u' values of p(t) or p(u)
};

using ReparameterizationCurves = std::vector<const Curve<double> *>;

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
