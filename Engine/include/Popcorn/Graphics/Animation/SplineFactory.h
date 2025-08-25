#pragma once

#include "CurveDefs.h"
#include "CurveFactory.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "SplineDefs.h"
#include "Splines.h"
#include <cassert>
#include <stdexcept>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class SplineFactory {
public:
#define KNOT_ASSERTIONS(KNOTS_NAME, REPARAMETERIZATION_CURVES)                 \
  do {                                                                         \
    assert((REPARAMETERIZATION_CURVES).empty() ||                              \
           (REPARAMETERIZATION_CURVES).size() == ((KNOTS_NAME).size() - 1));   \
    assert((KNOTS_NAME).front().t == 0.0 && "First knot must start at t = 0"); \
    for (size_t i = 1; i < (KNOTS_NAME).size(); ++i) {                         \
      auto &prev = (KNOTS_NAME)[i - 1];                                        \
      auto &curr = (KNOTS_NAME)[i];                                            \
      assert(prev.t >= 0.0 && prev.t < 1.0);                                   \
      assert(curr.t >= 0.0 && curr.t < 1.0);                                   \
      assert(prev.t < curr.t && "Knot 't's must be strictly increasing");      \
    }                                                                          \
  } while (0);

  template <CurveValueType T>
  // for rails
  const Spline<T> *
  MakeSpline(const SplineInfo_BlenderJSONExport_BezierForm<T> &splineInfo) {}

  // For splines made of cubic hermite curves (e.g. animations - gltf
  // cubicspline sampler type)
  template <CurveValueType T>
  const Spline<T> *MakeSpline(const std::vector<HermiteKnot<T>> &knots,
                              const ReparameterizationCurves &rpCurves = {}) {}

  // For splines made of linear curves (e.g. animations - gltf linear sampler
  // type)
  template <CurveValueType T>
  const Spline<T> *MakeSpline(const std::vector<LinearKnot<T>> &knots,
                              const ReparameterizationCurves &rpCurves = {}) {
    assert(knots.size() > 1 && "atleast 2 knots expected");
    KNOT_ASSERTIONS(knots, rpCurves)

    std::vector<SplineSegment<T>> segments;

    for (size_t i = 1; i < knots.size(); ++i) {
      const LinearKnot<T> &prev = knots[i - 1];
      const LinearKnot<T> &curr = knots[i];

      CurveInfoLinearForm<T> cInfo;
      cInfo.p0 = prev.val;
      cInfo.p1 = curr.val;

      const Curve<T> *c = CurveFactory::Get()->GetCurvePtr(cInfo);

      SplineSegment<T> seg{};
      seg.t = curr.t;
      seg.curve = c;
      seg.reparameterizationCurve =
          rpCurves.empty() ? nullptr : rpCurves[i - 1];

      segments.emplace_back(seg);
    }
  }

  // Real-time splines (offload to compute pipeline in the future)
  template <CurveValueType T>
  const Spline<T> *
  MakeAutomaticSpline(const std::vector<LinearKnot<T>> &knots,
                      const ReparameterizationCurves &rpCurves = {}) {
    // TODO: Check if hashed value exists, if so, return it.
    assert(knots.size() > 2 && "Catmull-Rom requires atleast 3 knots");
    KNOT_ASSERTIONS(knots, rpCurves)

    std::vector<LinearKnot<T>> tempKnots{};

    // First & last knots are "created" by linear extrapolations of the
    // natural-endpoint tangents. Hence 2 extra "phantom" knots.
    tempKnots.reserve(knots.size() + 2);

    // --- minus 1th (phantom)knot ---
    {
      const auto &P0 = knots[0].val;
      const auto &P1 = knots[1].val;
      tempKnots.push_back({
          P0 + (P0 - P1), // valueAtT(natural tangent at t=0)
          knots[0].t      // t value doesn't matter here, this knot is discarded
      });
    }

    // --- 0th to (n-1)th knots ---
    tempKnots.insert(tempKnots.end(), knots.begin(), knots.end());

    // --- nth(phantom) knot ---
    {
      size_t N = knots.size();
      const auto &Pn1 = knots[N - 1].val;
      const auto &Pn2 = knots[N - 2].val;
      tempKnots.push_back({
          Pn1 + (Pn1 - Pn2), // value at t(natural tangent at t=1)
          knots[N - 1].t // t value doesn't matter here, this knot is discarded
      });
    }

    // At this point, the spline has atleast 5 knots(min 3 + first & last)
    assert(tempKnots.size() > 4);

    std::vector<SplineSegment<T>> segments;
    segments.reserve(tempKnots.size() - 3);

    for (size_t i = 1; i + 2 < tempKnots.size(); ++i) {
      const T &k_i_minus1 = tempKnots[i - 1].val;
      const T &k_i = tempKnots[i].val;
      const T &k_i_plus1 = tempKnots[i + 1].val;
      const T &k_i_plus2 = tempKnots[i + 2].val;

      // c1 continuity
      T outVelocityAt_k_i = 0.5f * (k_i_plus1 - k_i_minus1);
      T inVelocityAt_k_i_plus1 = 0.5f * (k_i_plus2 - k_i);

      // create Hermite curve
      CurveInfoHermiteForm<T> curveInfoHermite;
      curveInfoHermite.p0 = k_i;
      curveInfoHermite.v0 = outVelocityAt_k_i;
      curveInfoHermite.v1 = inVelocityAt_k_i_plus1;
      curveInfoHermite.p1 = k_i_plus1;

      const Curve<T> *curve =
          CurveFactory::Get()->GetCurvePtr(curveInfoHermite);

      SplineSegment<T> seg;
      seg.curve = curve;
      seg.reparameterizationCurve =
          rpCurves.empty() ? nullptr : rpCurves[i - 1];
      seg.t = tempKnots[i].t;
      segments.emplace_back(seg);
    }

    Spline<T> s{segments};
  }
#undef KNOT_ASSERTIONS

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
