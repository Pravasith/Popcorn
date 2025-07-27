#pragma once

#include "Animation/CurveDefs.h"
#include "ContextGfx.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <CurveValueType T> struct Knot {
  T valueAtT;
  float t = .0f; // [0, 1]
};

template <CurveValueType T> struct SplineSegment {
  // TODO: just degree 3 for now bc curves are currently designed as cubic, add
  // quadratic & others later.
  const Curve<T> *curve;
  const Curve<float> *reparameterizationCurve = nullptr;
  float t0 = 0.0f, t1 = 1.0f; // segment start & end in terms of t
};

//
//
// -------------------------------------------------------------------
// --- MANUAL SPLINES ------------------------------------------------
// --- MANUAL SPLINES ------------------------------------------------
// --- MANUAL SPLINES ------------------------------------------------
// -------------------------------------------------------------------
//

//
// -------------------------------------------------------------------
// --- SPLINE --------------------------------------------------------
//
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
template <CurveValueType T> class Spline {
protected:
  // Default ctor not for making base Spline objects, but only for the derived
  // classes to use so they can build the custom segments from m_knots.
  Spline() = default;

public:
  explicit Spline(std::initializer_list<SplineSegment<T>> segs) {
    CreateSegments(segs);
  };

  virtual ~Spline() = default;

  virtual T GetValueAt_Fast(float t) const {
    assert(m_segments.size() != 0 && "No segments defined");

    auto [u, segmentPtr] = GetLocalParameterAndSegment(t);

    if (segmentPtr->reparameterizationCurve) {
      u = segmentPtr->reparameterizationCurve->GetValueAt_Fast(u);
    }

    return segmentPtr->curve->GetValueAt_Fast(u);
  }

  virtual T GetValueAt_Slow(float t) const {
    assert(m_segments.size() != 0 && "No segments defined");

    auto [u, segmentPtr] = GetLocalParameterAndSegment(t);

    if (segmentPtr->reparameterizationCurve) {
      u = segmentPtr->reparameterizationCurve->GetValueAt_Slow(u);
    }

    return segmentPtr->curve->GetValueAt_Slow(u);
  }

  virtual T GetFirstDerivativeAt_Fast(float t) const {
    assert(m_segments.size() != 0 && "No segments defined");

    auto [u, segmentPtr] = GetLocalParameterAndSegment(t);

    if (segmentPtr->reparameterizationCurve) {
      u = segmentPtr->reparameterizationCurve->GetValueAt_Fast(u);
    }

    return segmentPtr->curve->GetFirstDerivativeAt_Fast(u);
  }
  virtual T GetFirstDerivativeAt_Slow(float t) const {
    assert(m_segments.size() != 0 && "No segments defined");

    auto [u, segmentPtr] = GetLocalParameterAndSegment(t);

    if (segmentPtr->reparameterizationCurve) {
      u = segmentPtr->reparameterizationCurve->GetValueAt_Slow(u);
    }

    return segmentPtr->curve->GetFirstDerivativeAt_Slow(u);
  }

protected:
  void CreateSegments(std::initializer_list<SplineSegment<T>> segs) {
    assert(segs.size() > 0);

    for (auto const &sg : segs)
      assert(sg.curve != nullptr);

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

    m_segments.reserve(segs.size());
    m_segments.insert(m_segments.end(), segs.begin(), segs.end());

    std::sort(m_segments.begin(), m_segments.end(),
              [](auto const &a, auto const &b) { return a.t0 < b.t0; });
  };

private:
  std::pair<float, const SplineSegment<T> *>
  GetLocalParameterAndSegment(float t) const {
    t = std::clamp(t, 0.0f, 1.0f); // guarantee t in [0,1], to avoid floating
                                   // point error values like 0.99999 or 1.0001

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

// TODO: Later
template <CurveValueType T> class BSpline : public Spline<T> {
public:
  BSpline() {}

private:
  std::vector<T> m_knots;
};

//
// -------------------------------------------------------------------
// --- CATMULL-ROM-SPLINE --------------------------------------------
//

template <CurveValueType T> class CatmullRomSpline : public Spline<T> {
  using Spline<T>::m_segments;

public:
  CatmullRomSpline(const std::vector<Knot<T>> &knots,
                   const std::string &splineName,
                   const Curve<float> *reparameterizationCurve = nullptr) {
    if (knots.size() < 3) {
      throw std::runtime_error("Knots size should atleast 3. If you only have "
                               "2 knots, use a Curve instead.");
    }

    // NOTE: CreateSegments() checks other needed assertions anyway so no need
    // to reassert the parameter conditions

    // First & last knots are "created" by linear extrapolations of the
    // natural-endpoint tangents. Hence 2 extra "phantom" knots.
    m_knots.reserve(knots.size() + 2);

    // --- minus 1th (phantom)knot ---
    {
      const auto &P0 = knots[0].valueAtT;
      const auto &P1 = knots[1].valueAtT;

      m_knots.push_back({
          P0 + (P0 - P1), // valueAtT(natural tangent at t=0)
          knots[0].t      // t value
      });
    }

    // --- 0th to (n-1)th knots ---
    m_knots.insert(m_knots.end(), knots.begin(), knots.end());

    // --- nth(phantom) knot ---
    {
      size_t N = knots.size();

      const auto &Pn1 = knots[N - 1].valueAtT;
      const auto &Pn2 = knots[N - 2].valueAtT;

      m_knots.push_back({
          Pn1 + (Pn1 - Pn2), // valueAtT(natural tangent at t=1)
          knots[N - 1].t     // t value
      });
    }

    // At this point, the spline has atleast 5 knots(min 3 + first & last)
    assert(m_knots.size() > 4);
    m_segments.reserve(m_knots.size() - 3);

    for (size_t i = 1; i + 2 < m_knots.size(); ++i) {

      T &k_i_minus1 = m_knots[i - 1].valueAtT;
      T &k_i = m_knots[i].valueAtT;
      T &k_i_plus1 = m_knots[i + 1].valueAtT;
      T &k_i_plus2 = m_knots[i + 2].valueAtT;

      // c1 continuity
      T outVelocityAt_k_i = 0.5f * (k_i_plus1 - k_i_minus1);
      T inVelocityAt_k_i_plus1 = 0.5f * (k_i_plus2 - k_i);

      // create Hermite curve
      CurveInfoHermiteForm<T> curveInfoHermite;
      curveInfoHermite.p0 = k_i;
      curveInfoHermite.v0 = outVelocityAt_k_i;
      curveInfoHermite.v1 = inVelocityAt_k_i_plus1;
      curveInfoHermite.p1 = k_i_plus1;

      const Curve<T> *curve = ContextGfx::AppCurves->MakeCurve(
          splineName + "_span" + std::to_string(i), curveInfoHermite);

      // segments
      m_segments.emplace_back({
          curve,                   // curve ptr
          reparameterizationCurve, // reparameterization
          m_knots[i].t,            // t0
          m_knots[i + 1].t,        // t1
      });
    }
  }

private:
  std::vector<Knot<T>> m_knots;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
