#pragma once

#include "Animation/AnimationDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// -------------------------------------------------------------------
// --- MANUAL SPLINES ------------------------------------------------
// --- MANUAL SPLINES ------------------------------------------------
// --- MANUAL SPLINES ------------------------------------------------
// -------------------------------------------------------------------
//
template <CurveValueType T> struct SplineSegment {
  Curve<T> *curve;

  // --- Reparameterization ---
  // t goes through this (reparameterizationCurve) first, and then the result
  // value will be used in GetValueAt & GetDerivativeAt functions of the 'curve'
  //
  // TODO: just degree 3 for now bc curves are currently designed as cubic, add
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
  //
  // About 'explicit' from ChatGPT:
  //
  // Marking the constructor explicit prevents unintended, implicit conversions
  // from an initializer list into a Spline. Without explicit, you could
  // accidentally write something like:
  //
  // Spline<float> s = { {&curveA, nullptr, 0.0f, 0.5f}, ... };
  //
  // and the compiler would silently interpret the braced list as a call to your
  // one‑argument constructor. Making it explicit means you must write:
  //
  // Spline<float> s{ {&curveA, nullptr, 0.0f, 0.5f}, ... };
  // or
  // Spline<float> s( { {&curveA, nullptr, 0.0f, 0.5f}, ... } );
  //
  explicit Spline(std::initializer_list<SplineSegment<T>> segs) {
    m_segments.reserve(segs.size());
    m_segments.insert(m_segments.end(), segs.begin(), segs.end());

    for (auto const &sg : segs)
      assert(sg.curve != nullptr);

    std::sort(m_segments.begin(), m_segments.end(),
              [](auto const &a, auto const &b) { return a.t0 < b.t0; });

    // Ensure invariants:
    // 1. No duplicates - No two adjacent segments are exactly same
    // 2. Continuous segment (avoid gaps or overlaps) - The t0 of set si should
    //    match with t1 of set si-1
    // 3. The 1st element of set s0 is 0 & 2nd element of set sn-1 is 1

    for (size_t i = 1; i < m_segments.size(); ++i) {
      const auto &prev = m_segments[i - 1];
      const auto &cur = m_segments[i];

      // 1st assertion
      assert(!(std::fabs(cur.t0 - prev.t0) < PC_EPS &&
               std::fabs(cur.t1 - prev.t1) < PC_EPS) &&
             "Duplicate SplineSegment interval");

      // 2nd assertion
      assert(std::fabs(cur.t0 - prev.t1) < PC_EPS &&
             "Gap or overlap between adjacent segments");
    }

    // 3rd assertion
    assert(std::fabs(m_segments.front().t0 - 0.0f) < PC_EPS &&
           "Spline should start at t=0.0f"); // avoid floating point comparision
                                             // error, instead of --
                                             // assert(m_segments.front().t0 ==
                                             // 0.0f);
    assert(
        std::fabs(m_segments.back().t1 - 1.0f) < PC_EPS &&
        "Spline should end at t=1.0f"); // avoid floating point comparision,
                                        // instead of --
                                        // assert(m_segments.back().t1 == 1.0f);
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
//
// -------------------------------------------------------------------
// --- AUTOMATIC SPLINES ---------------------------------------------
// --- AUTOMATIC SPLINES ---------------------------------------------
// --- AUTOMATIC SPLINES ---------------------------------------------
// -------------------------------------------------------------------

//
// -------------------------------------------------------------------
// --- B-SPLINE ------------------------------------------------------
//

template <CurveValueType T> class BSpline : Spline<T> {
public:
  BSpline() {}
  ~BSpline() {}

private:
  std::vector<CurveInfoBezierForm<T>> m_knots;
};

//
// -------------------------------------------------------------------
// --- CATMULL-ROM-SPLINE --------------------------------------------
//

template <CurveValueType T> class CatmullRomSpline : Spline<T> {
public:
  CatmullRomSpline() {}
  ~CatmullRomSpline() {}

private:
  std::vector<CurveInfoBezierForm<T>> m_knots;
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
