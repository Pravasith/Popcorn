#pragma once

#include "AnimationDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <CurveValueType T> struct SplineSegment {
  Curve<T> *curve;
  Curve<float> *reparameterizationCurve =
      nullptr; // t goes through this first, and then the
               // result value will be used in GetValueAt &
               // GetDerivativeAt functions of the 'curve'
  float t0 = 0.0f, t1 = 0.1f;
};

//
// -------------------------------------------------------------------
// --- SPLINE --------------------------------------------------------
//
template <CurveValueType T> class Spline {
public:
public:
  Spline() = default;
  virtual ~Spline() = default;

  virtual T GetValueAt_Fast(float t) const = 0;
  virtual T GetValueAt_Slow(float t) const = 0;

  virtual T GetFirstDerivativeAt_Fast(float t) const = 0;
  virtual T GetFirstDerivativeAt_Slow(float t) const = 0;

protected:
  std::vector<SplineSegment<T>> m_segments{};
};

//
// -------------------------------------------------------------------
// --- CURVE --------------------------------------------------
//

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
