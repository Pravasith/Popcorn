#pragma once

#include "CurvesDefs.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cassert>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <string>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <ParametericFunctionOutputType T>
static inline constexpr T PC_Lerp(const T &p0, const T &p1, float t) {
  return p0 + (p1 - p0) * t;
};

//
// -------------------------------------------------------------------
// --- CURVE ---------------------------------------------------------
//
template <ParametericFunctionOutputType T = float> class Curve {
public:
  Curve() = default;
  virtual ~Curve() = default;

  virtual T GetValueAt_Fast(float t) const = 0;
  virtual T GetValueAt_Slow(float t) const = 0;

  virtual T GetFirstDerivativeAt_Fast(float t) const = 0;
  virtual T GetFirstDerivativeAt_Slow(float t) const = 0;

  // virtual T GetSecondDerivativeAt_Fast(float t) const = 0;
  // virtual T GetSecondDerivativeAt_Slow(float t) const = 0;
};

//
// -------------------------------------------------------------------
// --- LINEAR CURVE --------------------------------------------------
//
template <ParametericFunctionOutputType T> class LinearCurve : public Curve<T> {
public:
  LinearCurve(const CurveInfoLinearForm<T> &curveInfo)
      : m_curveInfo(curveInfo) {};
  virtual ~LinearCurve() override = default;

  virtual T GetValueAt_Fast(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);
    return GetValueAt_Slow(t);
  }
  virtual T GetValueAt_Slow(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);
    return PC_Lerp(m_curveInfo.p0, m_curveInfo.p1, t);
  }

  virtual T GetFirstDerivativeAt_Fast(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);
    return GetFirstDerivativeAt_Slow(t);
  }
  virtual T GetFirstDerivativeAt_Slow(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);
    return m_curveInfo.p1 - m_curveInfo.p0;
  }

private:
  CurveInfoLinearForm<T> m_curveInfo;
};

//
// -------------------------------------------------------------------
// --- BEZIER CURVE --------------------------------------------------
//
template <ParametericFunctionOutputType T> class BezierCurve : public Curve<T> {
public:
  BezierCurve(const CurveInfoBezierForm<T> &info)
      : // Bezier control points
        m_controlPoints{info.b0, info.b1, info.b2, info.b3},
        // Bezier -> monomial form conversion, ref -
        // https://www.gamemath.com/book/curves.html#cubic_bezier_monomial_form
        m_coefficients{info.b0, -3 * info.b0 + 3 * info.b1,
                       3 * info.b0 - 6 * info.b1 + 3 * info.b2,
                       -info.b0 + 3 * info.b1 - 3 * info.b2 + info.b3} {};
  virtual ~BezierCurve() override = default;

  // Berstein basis implementation
  virtual T GetValueAt_Fast(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);
    // Power-basis conversion (in the c'tor) + Horner's rule (just a notational
    // trick sorta like dynamic programming)
    T x = m_coefficients[3];
    x = x * t + m_coefficients[2];
    x = x * t + m_coefficients[1];
    return x * t + m_coefficients[0];
  };

  // DeCasteljau's algorithm implementation
  virtual T GetValueAt_Slow(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);
    int n = s_degree + 1; // 4 control points

    // For in-place evaluation
    T controlPtsCopy[4]{m_controlPoints[0], m_controlPoints[1],
                        m_controlPoints[2], m_controlPoints[3]};
    while (n-- > 1) // 3 times (3 = degree = (no. of m_controlPoints) - 1)
    {
      for (int i = 0; i < n; ++i) {
        controlPtsCopy[i] =
            PC_Lerp(controlPtsCopy[i], controlPtsCopy[i + 1], t);
      }
    }

    return controlPtsCopy[0];
  };

  virtual T GetFirstDerivativeAt_Fast(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);

    T x = m_coefficients[3] * 3.0f;
    x = x * t + m_coefficients[2] * 2.0f;
    return x * t + m_coefficients[1];
  }
  virtual T GetFirstDerivativeAt_Slow(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);

    // Derivative is a quadratic.
    // Build derivative control points of the quadratic from the control points
    // of the cubic with [ Qi = 3*(Pi+1 - Pi) ] -- Bernstein polynomial formula
    // from ref -
    // https://www.gamemath.com/book/curves.html#bernstein_basis

    T Q0 = (m_controlPoints[1] - m_controlPoints[0]) * 3.0f;
    T Q1 = (m_controlPoints[2] - m_controlPoints[1]) * 3.0f;
    T Q2 = (m_controlPoints[3] - m_controlPoints[2]) * 3.0f;

    // Usual DeCasteljau's algo - 1st round lerp
    T R0 = PC_Lerp(Q0, Q1, t);
    T R1 = PC_Lerp(Q1, Q2, t);

    // Usual DeCasteljau's algo - 2nd round lerp
    return PC_Lerp(R0, R1, t);
  }

private:
  static constexpr uint16_t s_degree = 3;

  T m_controlPoints[4]; // Bezier control points
  T m_coefficients[4];  // cubic polynomial co-efficients
};

//
// -------------------------------------------------------------------
// --- HERMITE CURVE -------------------------------------------------
//
template <ParametericFunctionOutputType T>
class HermiteCurve : public Curve<T> {
public:
  HermiteCurve(const CurveInfoHermiteForm<T> &curveInfo)
      : m_curveInfo(curveInfo) {};
  virtual ~HermiteCurve() override = default;

  virtual T GetValueAt_Fast(float t) const override final {};
  virtual T GetValueAt_Slow(float t) const override final {};

  virtual T GetFirstDerivativeAt_Fast(float t) const override final {};
  virtual T GetFirstDerivativeAt_Slow(float t) const override final {};

private:
  static constexpr uint16_t s_degree = 3;

  T m_controlPoints[4]; // Hermite control points
  T m_coefficients[4];  // cubic polynomial co-efficients

  CurveInfoHermiteForm<T> m_curveInfo;
};

//
// -------------------------------------------------------------------
// --- SPLINE --------------------------------------------------------
//
template <SplineForm V, ParametericFunctionOutputType ParamType> class Spline {
public:
  static constexpr SplineForm typeValue = V;

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
