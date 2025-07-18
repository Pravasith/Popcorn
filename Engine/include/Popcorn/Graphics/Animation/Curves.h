#pragma once

#include "AnimationDefs.h"
#include "GlobalMacros.h"
#include <cassert>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_float4x4.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <CurveValueType T>
static inline constexpr T PC_Lerp(const T &p0, const T &p1, float t) {
  return p0 + (p1 - p0) * t;
};

//
// -------------------------------------------------------------------
// --- CURVE ---------------------------------------------------------
//
template <CurveValueType T = float> class Curve {
public:
  Curve() = default;
  virtual ~Curve() = default;

  virtual T GetValueAt_Fast(float t) const = 0;
  virtual T GetValueAt_Slow(float t) const = 0;

  virtual T GetFirstDerivativeAt_Fast(float t) const = 0;
  virtual T GetFirstDerivativeAt_Slow(float t) const = 0;

  template <CurveInfoForms F> DeriveCurveInfoType<F, T> GetCurveInfoAs() const {
    if constexpr (F == CurveInfoForms::LinearForm) {
      // Convert to linear
    } else if constexpr (F == CurveInfoForms::BezierForm) {
      // Convert to Bezier
    } else if constexpr (F == CurveInfoForms::HermiteForm) {
      // Convert to Hermite
    }
  };

  // virtual T GetSecondDerivativeAt_Fast(float t) const = 0;
  // virtual T GetSecondDerivativeAt_Slow(float t) const = 0;
};

//
// -------------------------------------------------------------------
// --- LINEAR CURVE --------------------------------------------------
//
template <CurveValueType T> class LinearCurve : public Curve<T> {
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
template <CurveValueType T> class BezierCurve : public Curve<T> {
public:
  BezierCurve(const CurveInfoBezierForm<T> &info)
      : // Bezier control points
        m_controlPoints{info.b0, info.b1, info.b2, info.b3},
        // Coefficients (cubic monomial form)
        // Cubic bezier -> monomial form conversion, ref -
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
    // Just Horner's rule applied to monomial form coefficients
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

    // simple derivative of the cubic equation (calculus power rule)
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
    T q0 = (m_controlPoints[1] - m_controlPoints[0]) * 3.0f;
    T q1 = (m_controlPoints[2] - m_controlPoints[1]) * 3.0f;
    T q2 = (m_controlPoints[3] - m_controlPoints[2]) * 3.0f;

    // Usual DeCasteljau's algo - 1st round lerp
    T r0 = PC_Lerp(q0, q1, t);
    T r1 = PC_Lerp(q1, q2, t);

    // Usual DeCasteljau's algo - 2nd round lerp
    return PC_Lerp(r0, r1, t);
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
template <CurveValueType T> class HermiteCurve : public Curve<T> {
public:
  HermiteCurve(const CurveInfoHermiteForm<T> &curveInfo)
      : // Hermite values (p0, v0, v1, p1)
        m_hermiteValues{curveInfo.p0, curveInfo.v0, curveInfo.v1, curveInfo.p1},
        // Coefficients (cubic monomial form)
        // Cubic hermite -> monomial form conversion, ref -
        // https://www.gamemath.com/book/curves.html#hermite_to_monomial_first
        m_coefficients{curveInfo.p0, curveInfo.v0,
                       -3 * curveInfo.p0 - 2 * curveInfo.v0 - curveInfo.v1 +
                           3 * curveInfo.p1,
                       2 * curveInfo.p0 + curveInfo.v0 + curveInfo.v1 -
                           2 * curveInfo.p1} {};
  virtual ~HermiteCurve() override = default;

  virtual T GetValueAt_Fast(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);

    // Just Horner's rule applied to monomial form coefficients
    T x = m_coefficients[3];
    x = x * t + m_coefficients[2];
    x = x * t + m_coefficients[1];
    return x * t + m_coefficients[0];
  };
  virtual T GetValueAt_Slow(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);

    // Hermite basis direct formula plug in (Hermite basis functions as
    // "weights" for barycentric coordinates) -- ref -
    // https://www.gamemath.com/book/curves.html#hermite_basis

    float tt = t * t;
    float ttt = tt * t;

    // Hermite basis functions
    float h00 = 2 * ttt - 3 * tt + 1;
    float h10 = ttt - 2 * tt + t;
    float h01 = -2 * ttt + 3 * tt;
    float h11 = ttt - tt;

    // m_hermiteValues = { p0, v0, v1, p1 }
    const T &p0 = m_hermiteValues[0];
    const T &v0 = m_hermiteValues[1];
    const T &v1 = m_hermiteValues[2];
    const T &p1 = m_hermiteValues[3];

    return p0 * h00 + v0 * h10 + p1 * h01 + v1 * h11;
  };

  virtual T GetFirstDerivativeAt_Fast(float t) const override final {
    assert(0.0f <= t && t <= 1.0f);

    T d = 3 * m_coefficients[3];       // 3*c3
    d = d * t + 2 * m_coefficients[2]; // 3*c3*t + 2*c2
    return d * t +                     // (3*c3*t + 2*c2)*t
           m_coefficients[1];          // + c1
  };
  virtual T GetFirstDerivativeAt_Slow(float t) const override final {
    assert(t >= 0 && t <= 1);

    // Derivating the Hermite basis functions and using them instead of the
    // exact Hermite basis functions (like in GetValueAt_Slow function above)

    float tt = t * t;

    // basis‐derivatives:
    float dh00 = 6 * tt - 6 * t;
    float dh10 = 3 * tt - 4 * t + 1;
    float dh01 = -6 * tt + 6 * t;
    float dh11 = 3 * tt - 2 * t;

    const T &p0 = m_hermiteValues[0];
    const T &v0 = m_hermiteValues[1];
    const T &v1 = m_hermiteValues[2];
    const T &p1 = m_hermiteValues[3];

    // blend:
    return p0 * dh00 + v0 * dh10 + p1 * dh01 + v1 * dh11;
  };

private:
  static constexpr uint16_t s_degree = 3;

  T m_hermiteValues[4]; // Hermite values (p0, v0, v1, p1)
  T m_coefficients[4];  // cubic polynomial co-efficients
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
