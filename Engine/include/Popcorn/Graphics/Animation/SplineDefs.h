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

template <CurveValueType T> struct SimpleKnot {
  T val;
  double t; // [0, 1)
};

//
// For splines made of cubic hermite curves (e.g. animations - gltf cubicspline
// sampler type)
template <CurveValueType T>
using SplineInfo_BlenderAnimations_HermiteKnots = std::vector<HermiteKnot<T>>;
//
// For splines made of linear curves (e.g. animations - linear sampler type)
template <CurveValueType T>
using SplineInfo_BlenderAnimations_LinearKnots = std::vector<SimpleKnot<T>>;

// For physical representation of curves (e.g. rails)
template <CurveValueType T> struct SplineInfo_BlenderJSONExport_BezierForm {
  std::vector<CurveInfoBezierForm<T>> bezierCurveInfoValues;
  std::vector<double> tValues; // 't' or 'u' values of p(t) or p(u)
};

// Real-time splines (offload to compute pipeline in the future)
template <CurveValueType T>
using SplineInfo_CatmullRom_LinearKnots = std::vector<SimpleKnot<T>>;

using ReparameterizationCurves = std::vector<const Curve<double> *>;

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
