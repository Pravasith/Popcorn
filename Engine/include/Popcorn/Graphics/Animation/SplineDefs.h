#pragma once

#include "CurveDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "MathConstants.h"

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

// TODO: just degree 3 for now bc curves are currently designed as cubic, add
// quadratic & others later.
template <CurveValueType T> struct SplineSegment {
  const Curve<T> *curve;
  const Curve<double> *reparameterizationCurve = nullptr;
  double t = 0.0; // [0, 1)
  double invLen = 0.0;
  void AssertInvariants() const {
    assert(curve && "curve in SplineSegment nullptr");
    assert(t >= 0.0 && t < 1.0);
  }
};

inline double PC_GetInvLen(double board, double dest) {
  double len = dest - board;
  // Treat tiny spans as instant (avoids 1/very-small blowing up)
  double invLen = (len > PC_EPS_D) ? 1.0 / len : 0.0;
  return invLen;
}

inline float PC_GetInvLen(float board, float dest) {
  float len = dest - board;
  // Treat tiny spans as instant (avoids 1/very-small blowing up)
  float invLen = (len > PC_EPS) ? 1.0 / len : 0.0;
  return invLen;
}

static inline void PC_Clamp_01(float &n) {
  n = n < 0.f ? 0.f : (n >= 1.f ? std::nextafter(1.f, 0.f) : n);
}
static inline void PC_Clamp_01(double &n) {
  n = n < 0.0 ? 0.0 : (n >= 1.0 ? std::nextafter(1.0, 0.0) : n);
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
