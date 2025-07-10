#pragma once

#include "CurvesDefs.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <string>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// -------------------------------------------------------------------
// --- CURVE ---------------------------------------------------------
//
template <ValidCurveParamType T> class Curve {
public:
  Curve() = default;
  virtual ~Curve() = default;

  // TODO: Check for out of bounds (0 < t < 1)
  virtual float GetValueAt_Fast(float t) = 0;
  virtual double GetValueAt_Slow(float t) = 0;

  // return (1 - t) * curveInfo.p0 + t * curveInfo.p1;
};

//
// -------------------------------------------------------------------
// --- BEZIER CURVE --------------------------------------------------
//
template <ValidCurveParamType T> class BezierCurve : public Curve<T> {
public:
  BezierCurve(const BezierInfo<T> &bezierCreateInfo)
      : m_data(bezierCreateInfo) {};
  virtual ~BezierCurve() override;

  virtual float GetValueAt_Fast(float t) override final;
  virtual double GetValueAt_Slow(float t) override final;

private:
  BezierInfo<T> m_data;
};

//
// -------------------------------------------------------------------
// --- HERMITE CURVE -------------------------------------------------
//
template <ValidCurveParamType T> class HermiteCurve : public Curve<T> {
public:
  HermiteCurve(const HermiteInfo<T> &bezierCreateInfo)
      : m_data(bezierCreateInfo) {};
  virtual ~HermiteCurve() override;

  virtual float GetValueAt_Fast(float t) override final;
  virtual double GetValueAt_Slow(float t) override final;

private:
  HermiteInfo<T> m_data;
};

//
// -------------------------------------------------------------------
// --- SPLINE --------------------------------------------------------
//
template <SplineType V, ValidCurveParamType ParamType> class Spline {
public:
  static constexpr SplineType typeValue = V;

public:
  Spline() { PC_PRINT("CREATED", TagType::Constr, "Spline") };
  ~Spline() { PC_PRINT("DESTROYED", TagType::Destr, "Spline") };

protected:
  // TODO: Change CurveType based on SplineType
  std::vector<Curve<ParamType> *> m_curveSegments{};
};

//
// -------------------------------------------------------------------
// --- POLYCURVE -----------------------------------------------------
//
// Contains Spline,

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
