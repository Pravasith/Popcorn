#pragma once

#include "GlobalMacros.h"
#include "SplineDefs.h"
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
