#pragma once

#include "AnimationDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <CurveValueType T> struct SplineSegment {
  Curve<T> *curve;

  // --- Reparameterization ---
  // t goes through this (reparameterizationCurve) first, and then the result
  // value will be used in GetValueAt & GetDerivativeAt functions of the 'curve'
  //
  // NOTE: just degree 3 for now bc curves are currently designed as cubic, add
  // quadratic & others later.
  Curve<float> *reparameterizationCurve = nullptr;

  float t0 = 0.0f, t1 = 1.0f; // segment start & end in terms of t
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

  virtual T GetValueAt_Fast(float t) const {
    auto [u, segmentPtr] = GetLocalParameterAndSegment(t);

    if (segmentPtr->reparameterizationCurve) {
      u = segmentPtr->reparameterizationCurve->GetValueAt_Fast(u);
    }

    return segmentPtr->curve->GetValueAt_Fast(u);
  }

  virtual T GetValueAt_Slow(float t) const {
    auto [u, segmentPtr] = GetLocalParameterAndSegment(t);

    if (segmentPtr->reparameterizationCurve) {
      u = segmentPtr->reparameterizationCurve->GetValueAt_Slow(u);
    }

    return segmentPtr->curve->GetValueAt_Slow(u);
  }

  virtual T GetFirstDerivativeAt_Fast(float t) const {

    auto [u, segmentPtr] = GetLocalParameterAndSegment(t);

    if (segmentPtr->reparameterizationCurve) {
      u = segmentPtr->reparameterizationCurve->GetValueAt_Fast(u);
    }

    return segmentPtr->curve->GetFirstDerivativeAt_Fast(u);
  }
  virtual T GetFirstDerivativeAt_Slow(float t) const {
    auto [u, segmentPtr] = GetLocalParameterAndSegment(t);

    if (segmentPtr->reparameterizationCurve) {
      u = segmentPtr->reparameterizationCurve->GetValueAt_Slow(u);
    }

    return segmentPtr->curve->GetFirstDerivativeAt_Slow(u);
  }

protected:
  std::pair<float, const SplineSegment<T> *>
  GetLocalParameterAndSegment(float t) const {
    t = std::clamp(
        t, 0.0f, 1.0f); // guarantee t in [0,1], to avoid floating point errors

    assert(0.0f <= t && 1.0f >= t);
    assert(!m_segments.empty());

    auto it = std::find_if(
        m_segments.begin(), m_segments.end(), [this, t](auto const &sgmt) {
          return (sgmt.t0 <= t && t < sgmt.t1) ||
                 (&sgmt == &this->m_segments.back() && t == 1.0f);
        });

    if (it == m_segments.end()) {
      throw std::runtime_error("No segment for t");
    }

    const SplineSegment<T> &segment = *it;

    assert(segment.t1 - segment.t0 > 0.0f); // avoid div by 0

    float u = (t - segment.t0) /
              (segment.t1 - segment.t0); // local parameter of the curve

    assert(0.0f <= u && 1.0f >= u);

    return std::make_pair(u, &segment);
  };

protected:
  std::vector<SplineSegment<T>> m_segments{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
