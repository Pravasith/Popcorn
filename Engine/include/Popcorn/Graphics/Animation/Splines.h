#pragma once

#include "Animation/CurveDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

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

template <CurveValueType T> class Spline {
public:
  using value_type = T;

public:
  explicit Spline(std::vector<SplineSegment<T>> segs) {
    CreateSegments(std::move(segs));
  };
  ~Spline() = default;

  T GetValueAt_Fast(float t) const {
    PC_Clamp_01(t);
    const SplineSegment<T> *segmentPtr = GetLocalSegment(t);
    float u = (t - segmentPtr->t) * segmentPtr->invLen;

    if (segmentPtr->reparameterizationCurve) {
      PC_Clamp_01(u);
      u = segmentPtr->reparameterizationCurve->GetValueAt_Fast(u);
    }
    PC_Clamp_01(u);
    return segmentPtr->curve->GetValueAt_Fast(u);
  }
  T GetValueAt_Slow(double t) const {
    PC_Clamp_01(t);
    const SplineSegment<T> *segmentPtr = GetLocalSegment(t);
    double u = (t - segmentPtr->t) * segmentPtr->invLen;

    if (segmentPtr->reparameterizationCurve) {
      PC_Clamp_01(u);
      u = segmentPtr->reparameterizationCurve->GetValueAt_Slow(u);
    }
    PC_Clamp_01(u);
    return segmentPtr->curve->GetValueAt_Slow(u);
  }

  T GetFirstDerivativeAt_Fast(float t) const {
    PC_Clamp_01(t);
    const SplineSegment<T> *segmentPtr = GetLocalSegment(t);
    float u = (t - segmentPtr->t) * segmentPtr->invLen;
    float r_du = 1.f; // reparameterization curve derivative at u

    if (segmentPtr->reparameterizationCurve) {
      PC_Clamp_01(u);
      // Because reparam curve is dependent on u which is dependent on t
      r_du = segmentPtr->reparameterizationCurve->GetFirstDerivativeAt_Fast(u);
      u = segmentPtr->reparameterizationCurve->GetValueAt_Fast(u);
    }
    PC_Clamp_01(u);
    // TODO: Learn about chain rule
    return segmentPtr->curve->GetFirstDerivativeAt_Fast(u) *
           (segmentPtr->invLen * r_du);
  }
  T GetFirstDerivativeAt_Slow(double t) const {
    PC_Clamp_01(t);
    const SplineSegment<T> *segmentPtr = GetLocalSegment(t);
    double u = (t - segmentPtr->t) * segmentPtr->invLen;
    double r_du = 1.0; // reparameterization curve derivative at u

    if (segmentPtr->reparameterizationCurve) {
      PC_Clamp_01(u);
      // Because reparam curve is dependent on u which is dependent on t
      r_du = segmentPtr->reparameterizationCurve->GetFirstDerivativeAt_Slow(u);
      u = segmentPtr->reparameterizationCurve->GetValueAt_Slow(u);
    }
    PC_Clamp_01(u);
    // TODO: Learn about chain rule
    return segmentPtr->curve->GetFirstDerivativeAt_Slow(u) *
           (segmentPtr->invLen * r_du);
  }

public:
  void CreateSegments(std::vector<SplineSegment<T>> segs) {
    assert(segs.size() > 1 && "Use a Curve if you only need 1 segment");

    for (const auto &sg : segs)
      sg.AssertInvariants();

    std::sort(segs.begin(), segs.end(),
              [](const SplineSegment<T> &a, const SplineSegment<T> &b) {
                return a.t < b.t;
              });

    assert(segs.front().t == 0.0 && "First segment must start at t = 0");

    for (size_t i = 1; i < segs.size(); ++i) {
      auto &prevSeg = segs[i - 1];
      auto &seg = segs[i];

      assert(prevSeg.t < seg.t && "Segment starts must be strictly increasing");
      prevSeg.invLen = PC_GetInvLen(prevSeg.t, seg.t);

      if (i == segs.size() - 1) {
        seg.invLen = PC_GetInvLen(seg.t, 1.0);
      }
    }

    m_segments = std::move(segs);
  };

private:
  const SplineSegment<T> *GetLocalSegment(double t) const {
    assert(t >= 0.0 && t < 1.0);

    // no duplicate t's anyway
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

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
