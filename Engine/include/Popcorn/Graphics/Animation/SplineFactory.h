#pragma once

#include "CurveDefs.h"
#include "CurveFactory.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "RailsUtils.h"
#include "SplineDefs.h"
#include "Splines.h"
#include <cassert>
#include <map>
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
    assert((KNOTS_NAME).back().t == 1.0 && "Last knot must end at t = 1");     \
    for (size_t i = 1; i < (KNOTS_NAME).size(); ++i) {                         \
      auto &prev = (KNOTS_NAME)[i - 1];                                        \
      auto &curr = (KNOTS_NAME)[i];                                            \
      assert(prev.t >= 0.0 && prev.t <= 1.0);                                  \
      assert(curr.t >= 0.0 && curr.t <= 1.0);                                  \
      assert(prev.t < curr.t && "Knot 't's must be strictly increasing");      \
    }                                                                          \
  } while (0);

  // For physical representation of curves (e.g. rails)
  template <CurveValueType T>
  [[nodiscard]] const Spline<T> *
  MakeSpline(const std::vector<CurveInfoBezierForm<T>> &bezierCurveInfoValues,
             const std::vector<double> &keyframeTs,
             const ReparameterizationCurves &rpCurves = {}) {
    assert(bezierCurveInfoValues.size() > 1 && keyframeTs.size() > 1 &&
           "atleast 2 knots expected");
    assert(bezierCurveInfoValues.size() == keyframeTs.size());
    assert(rpCurves.empty() || bezierCurveInfoValues.size() == rpCurves.size());

    assert(keyframeTs.front() == 0.0 && "First knot must start at t = 0");
    assert(keyframeTs.back() < 1.0 && "Last segment start must be < 1.0");

    for (size_t i = 1; i < keyframeTs.size(); ++i) {
      auto prev = keyframeTs[i - 1];
      auto curr = keyframeTs[i];
      assert(prev < curr && "Knot 't's must be strictly increasing");
      assert(prev >= 0.0 && prev <= 1.0);
      assert(curr >= 0.0 && curr <= 1.0);
    }

    std::vector<SplineSegment<T>> segments;

    for (size_t i = 0; i < bezierCurveInfoValues.size(); ++i) {
      CurveInfoBezierForm<T> cInfo;
      cInfo.b0 = bezierCurveInfoValues[i].b0;
      cInfo.b1 = bezierCurveInfoValues[i].b1;
      cInfo.b2 = bezierCurveInfoValues[i].b2;
      cInfo.b3 = bezierCurveInfoValues[i].b3;

      const Curve<T> *c = CurveFactory::Get()->GetCurvePtr(cInfo);
      SplineSegment<T> seg{};
      seg.t = keyframeTs[i];
      seg.curve = c;
      seg.reparameterizationCurve = rpCurves.empty() ? nullptr : rpCurves[i];
      segments.emplace_back(seg);
    }

    return AddSplineToBank(std::move(segments));
  }

  // For splines made of cubic hermite curves (e.g. animations - gltf
  // cubicspline sampler type)
  template <CurveValueType T>
  [[nodiscard]] const Spline<T> *
  MakeSpline(const std::vector<HermiteKnot<T>> &knots,
             const ReparameterizationCurves &rpCurves = {}) {
    assert(knots.size() > 1 && "atleast 2 knots expected");
    KNOT_ASSERTIONS(knots, rpCurves)

    std::vector<SplineSegment<T>> segments;

    for (size_t i = 1; i < knots.size(); ++i) {
      const HermiteKnot<T> &prev = knots[i - 1];
      const HermiteKnot<T> &curr = knots[i];

      CurveInfoHermiteForm<T> cInfo;
      cInfo.p0 = prev.val;
      cInfo.v0 = prev.vOut;
      cInfo.v1 = curr.vIn;
      cInfo.p1 = curr.val;

      const Curve<T> *c = CurveFactory::Get()->GetCurvePtr(cInfo);

      SplineSegment<T> seg{};
      seg.t = prev.t;
      seg.curve = c;
      seg.reparameterizationCurve =
          rpCurves.empty() ? nullptr : rpCurves[i - 1];

      segments.emplace_back(seg);
    }

    return AddSplineToBank(std::move(segments));
  }

  // For splines made of linear curves (e.g. animations - gltf linear sampler
  // type)
  template <CurveValueType T>
  [[nodiscard]] const Spline<T> *
  MakeSpline(const std::vector<LinearKnot<T>> &knots,
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
      seg.t = prev.t;
      seg.curve = c;
      seg.reparameterizationCurve =
          rpCurves.empty() ? nullptr : rpCurves[i - 1];

      segments.emplace_back(seg);
    }

    return AddSplineToBank(std::move(segments));
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

    return AddSplineToBank(std::move(segments));
  }
#undef KNOT_ASSERTIONS

private:
  template <CurveValueType T>
  const Spline<T> *AddSplineToBank(std::vector<SplineSegment<T>> &&segments) {
    SplineHashType hash = PC_HashSplineSegment(segments);
    if constexpr (std::is_same_v<T, float>) {
      auto [it, _isInserted] =
          m_floatSplines.try_emplace(hash, std::move(segments));
      return &it->second;
    } else if constexpr (std::is_same_v<T, glm::vec2>) {
      auto [it, _isInserted] =
          m_vec2Splines.try_emplace(hash, std::move(segments));
      return &it->second;
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
      auto [it, _isInserted] =
          m_vec3Splines.try_emplace(hash, std::move(segments));
      return &it->second;
    } else if constexpr (std::is_same_v<T, glm::vec4>) {
      auto [it, _isInserted] =
          m_vec4Splines.try_emplace(hash, std::move(segments));
      return &it->second;
    }
  }

public:
  [[nodiscard]] inline static SplineFactory *Get() {
    if (s_instance) {
      return s_instance;
    }

    s_instance = new SplineFactory();
    return s_instance;
  }

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of SplineFactory")
    };
  }

private:
  SplineFactory() { PC_PRINT("CREATED", TagType::Constr, "SplineFactory") };
  ~SplineFactory() {
    CleanUp();
    PC_PRINT("DESTROYED", TagType::Destr, "SplineFactory")
  };

  SplineFactory(const SplineFactory &) = delete;
  SplineFactory &operator=(const SplineFactory &) = delete;
  SplineFactory(SplineFactory &&) = delete;
  SplineFactory &operator=(SplineFactory &&) = delete;

private:
  void CleanUp() {
    m_floatSplines.clear();
    m_vec2Splines.clear();
    m_vec3Splines.clear();
    m_vec4Splines.clear();
  }

private:
  static SplineFactory *s_instance;

  std::map<SplineHashType, Spline<float>> m_floatSplines;
  std::map<SplineHashType, Spline<glm::vec2>> m_vec2Splines;
  std::map<SplineHashType, Spline<glm::vec3>> m_vec3Splines;
  std::map<SplineHashType, Spline<glm::vec4>> m_vec4Splines;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
