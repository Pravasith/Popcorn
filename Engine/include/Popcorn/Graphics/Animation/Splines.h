#pragma once

#include "AnimationDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <initializer_list>
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
  Spline(std::initializer_list<SplineSegment<T>> segs) : m_segments(segs) {
    for (auto const &sg : segs)
      assert(sg.curve != nullptr);

    std::sort(m_segments.begin(), m_segments.end(),
              [](auto const &a, auto const &b) { return a.t0 < b.t0; });

    // Ensure invariants:
    // 1. No duplicates - No two adjacent segments are exactly same
    // 2. Continuous segment (avoid gaps or overlaps) - The t0 of set si should
    //    match with t1 of set si-1
    // 3. The 1st element of set s0 is 0 & 2nd element of set sn-1 is 1

    for (int i = 1; i < m_segments.size(); ++i) {
      // 1st assertion
      assert((m_segments[i].t0 != m_segments[i - 1].t0) &&
             (m_segments[i].t1 != m_segments[i - 1].t1));

      // 2nd assertion
      assert(m_segments[i].t0 == m_segments[i - 1].t1);
    }

    // 3rd assertion
    assert(m_segments.front().t0 == 0.0f);
    assert(m_segments.back().t1 == 1.0f);
  };

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

//
// -------------------------------------------------------------------
// --- B-SPLINE ------------------------------------------------------
//

//
// -------------------------------------------------------------------
// --- CATMULL-ROM-SPLINE --------------------------------------------
//

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
