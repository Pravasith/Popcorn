#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "SplineDefs.h"
#include "Splines.h"
#include <cassert>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <CurveValueType T> class SplineBank {};

class SplineFactory {
public:
  template <CurveValueType T>
  // for rails
  const Spline<T> *
  GetSpline(const SplineInfo_BlenderJSONExport_BezierForm<T> &splineInfo) {}

  // for animations
  template <CurveValueType T>
  const Spline<T> *
  GetSpline(const SplineInfo_BlenderAnimations_HermiteKnots<T> &splineInfo) {}
  template <CurveValueType T>
  const Spline<T> *
  GetSpline(const SplineInfo_BlenderAnimations_LinearKnots<T> &splineInfo) {}

  // real-time splines (for compute shader)
  template <CurveValueType T>
  const Spline<T> *
  MakeAutomaticSpline(const SplineInfo_CatmullRom_LinearKnots<T> &linearKnots,
                      const ReparameterizationCurves &rpCurves = {}) {
    // TODO: Check if hashed value exists, if so, return it.
    if (linearKnots.size() < 3) {
      throw std::runtime_error("Catmull-Rom requires atleast 3 knots");
    }
    if (rpCurves.size()) {
      assert(rpCurves.size() == linearKnots.size() - 1);
    }

    std::sort(linearKnots.begin(), linearKnots.end(),
              [](const SimpleKnot<T> &a, const SimpleKnot<T> &b) {
                return a.t < b.t;
              });

    assert(linearKnots.front().t == 0.0 && "First knot must start at t = 0");

    for (size_t i = 1; i < linearKnots.size(); ++i) {
      auto &prev = linearKnots[i - 1];
      auto &curr = linearKnots[i];

      assert(prev.t < curr.t && "Knot 't's must be strictly increasing");
    }

    std::vector<SimpleKnot<T>> knots;

    // First & last knots are "created" by linear extrapolations of the
    // natural-endpoint tangents. Hence 2 extra "phantom" knots.
    knots.reserve(linearKnots.size() + 2);

    // --- minus 1th (phantom)knot ---
    {
      const auto &P0 = knots[0].val;
      const auto &P1 = knots[1].val;

      knots.push_back({
          P0 + (P0 - P1),  // valueAtT(natural tangent at t=0)
          linearKnots[0].t // t value
      });
    }

    // --- 0th to (n-1)th knots ---
    knots.insert(knots.end(), linearKnots.begin(), linearKnots.end());

    // --- nth(phantom) knot ---
    {
      size_t N = linearKnots.size();

      const auto &Pn1 = linearKnots[N - 1].val;
      const auto &Pn2 = linearKnots[N - 2].val;

      knots.push_back({
          Pn1 + (Pn1 - Pn2),   // value at t(natural tangent at t=1)
          linearKnots[N - 1].t // t value
      });
    }

    // At this point, the spline has atleast 5 knots(min 3 + first & last)
    assert(knots.size() > 4);

    std::vector<SplineSegment<T>> segments;

    segments.reserve(knots.size() - 3);

    for (size_t i = 1; i + 2 < knots.size(); ++i) {
      T &k_i_minus1 = knots[i - 1].valueAtT;
      T &k_i = knots[i].valueAtT;
      T &k_i_plus1 = knots[i + 1].valueAtT;
      T &k_i_plus2 = knots[i + 2].valueAtT;

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

      SplineSegment<T> seg;
      seg.curve = curve;
      seg.reparameterizationCurve = rpCurves[i - 1];
      seg.t = knots[i].t;

      segments.emplace_back(seg);
    }

    Spline<T> s{segments};
  }

public:
  [[nodiscard]] inline static SplineFactory *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new SplineFactory();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of SplineFactory")
    };
  };

private:
  SplineFactory() { PC_PRINT("CREATED", TagType::Constr, "SplineFactory") };
  ~SplineFactory() { PC_PRINT("DESTROYED", TagType::Destr, "SplineFactory") };

  SplineFactory(const SplineFactory &) = delete;
  SplineFactory &operator=(const SplineFactory &) = delete;
  SplineFactory(SplineFactory &&) = delete;
  SplineFactory &operator=(SplineFactory &&) = delete;

private:
  static SplineFactory *s_instance;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
