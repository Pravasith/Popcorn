#pragma once

#include "CurveDefs.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Splines.h"

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
  GetSpline(const SplineInfo_BlenderAnimations_HermiteForm<T> &splineInfo) {}
  template <CurveValueType T>
  const Spline<T> *
  GetSpline(const SplineInfo_BlenderAnimations_LinearForm<T> &splineInfo) {}

  // real-time splines (for compute shader)
  template <CurveValueType T>
  const Spline<T> *
  GetSpline(const SplineInfo_Automatic_CatmullRom<T> &splineInfo) {}

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
