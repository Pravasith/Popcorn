#pragma once

#include "Animation/CurveDefs.h"
#include "CurveFactory.h"
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
  double t = 0.0; // [0, 1]
};

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
template <CurveValueType T> class Spline {
public:
  using value_type = T;

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
    const SplineSegment<T> *segmentPtr = GetLocalSegment(t);
    float u = (t - segmentPtr->t) * segmentPtr->invLen;

    if (segmentPtr->reparameterizationCurve) {
      PC_Clamp_01(u);
      u = segmentPtr->reparameterizationCurve->GetValueAt_Fast(u);
    }
    PC_Clamp_01(u);
    return segmentPtr->curve->GetValueAt_Fast(u);
  }
  virtual T GetValueAt_Slow(double t) const {
    const SplineSegment<T> *segmentPtr = GetLocalSegment(t);
    double u = (t - segmentPtr->t) * segmentPtr->invLen;

    if (segmentPtr->reparameterizationCurve) {
      PC_Clamp_01(u);
      u = segmentPtr->reparameterizationCurve->GetValueAt_Slow(u);
    }
    PC_Clamp_01(u);
    return segmentPtr->curve->GetValueAt_Slow(u);
  }

  virtual T GetFirstDerivativeAt_Fast(float t) const {
    const SplineSegment<T> *segmentPtr = GetLocalSegment(t);
    float u = (t - segmentPtr->t) * segmentPtr->invLen;
    if (segmentPtr->reparameterizationCurve) {
      PC_Clamp_01(u);
      u = segmentPtr->reparameterizationCurve->GetValueAt_Fast(u);
    }
    PC_Clamp_01(u);
    return segmentPtr->curve->GetFirstDerivativeAt_Fast(u);
  }
  virtual T GetFirstDerivativeAt_Slow(double t) const {
    const SplineSegment<T> *segmentPtr = GetLocalSegment(t);
    double u = (t - segmentPtr->t) * segmentPtr->invLen;

    if (segmentPtr->reparameterizationCurve) {
      PC_Clamp_01(u);
      u = segmentPtr->reparameterizationCurve->GetValueAt_Slow(u);
    }
    PC_Clamp_01(u);
    return segmentPtr->curve->GetFirstDerivativeAt_Slow(u);
  }

protected:
  void CreateSegments(std::initializer_list<SplineSegment<T>> segs) {
    assert(segs.size() > 1 && "Use a Curve if you only need 1 segment");

    std::vector<SplineSegment<T>> v{segs};
    for (const auto &sg : v)
      sg.AssertInvariants();

    std::sort(v.begin(), v.end(),
              [](const SplineSegment<T> &a, const SplineSegment<T> &b) {
                return a.t < b.t;
              });

    assert(v.front().t == 0.0 && "First segment must start at t = 0");

    for (size_t i = 1; i < v.size(); ++i) {
      auto &prevSeg = v[i - 1];
      auto &seg = v[i];

      assert(prevSeg.t < seg.t && "Segment starts must be strictly increasing");
      prevSeg.invLen = PC_GetInvLen(prevSeg.t, seg.t);

      if (i == v.size() - 1) {
        seg.invLen = PC_GetInvLen(seg.t, 1.0);
      }
    }

    m_segments = std::move(v);
  };

private:
  const SplineSegment<T> *GetLocalSegment(double t) const {
    assert(t >= 0.0 && t < 1.0);

    auto it = std::upper_bound(
        m_segments.begin(), m_segments.end(), t,
        [](double val, const SplineSegment<T> &seg) { return val < seg.t; });

    if (it == m_segments.begin()) {
      return &*it;
    }
    return &*(it - 1);
  }

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
// --- CATMULL-ROM-SPLINE --------------------------------------------
//

template <CurveValueType T> class CatmullRomSpline : public Spline<T> {
  using Spline<T>::m_segments;

public:
  CatmullRomSpline(const std::vector<Knot<T>> &knots,
                   const std::string &splineName,
                   const Curve<float> *reparameterizationCurve = nullptr) {
    if (knots.size() < 3) {
      throw std::runtime_error("Catmull-Rom requires atleast 3 knots");
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

      // const Curve<T> *curve = ContextGfx::AppCurves->GetCurvePtr(
      const Curve<T> *curve = CurveFactory::Get()->GetCurvePtr(
          // splineName + "_span" + std::to_string(i),
          curveInfoHermite);

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
