#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <concepts>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum CurveType {
  Linear,
  Hermite,
  Bezier,
};

enum SplineType {
  CatmullRom,
  BSpline,
};

template <typename T>
concept ValidCurveParamType = std::copyable<T> && requires(T a, T b, float c) {
  // a & b 'type' requirements
  { a + b } -> std::convertible_to<T>;
  { a - b } -> std::convertible_to<T>;
  { a * b } -> std::convertible_to<T>;
  { a / b } -> std::convertible_to<T>;

  // should be multiplyable by floats (c in this case)
  { a * c } -> std::convertible_to<T>;
  { c * a } -> std::convertible_to<T>;
};

template <ValidCurveParamType T> struct LinearInfo {
  T p0;
  T p1;
};

template <ValidCurveParamType T> struct BezierInfo {
  T b0;
  T b1;
  T b2;
  T b3;
};

template <ValidCurveParamType T> struct HermiteInfo {
  T p1;
  T v1;
  T v2;
  T p2;
};

template <CurveType T, ValidCurveParamType ParamType>
struct DeriveCurveInfoType;

template <ValidCurveParamType ParamType>
struct DeriveCurveInfoType<CurveType::Linear, ParamType> {
  using type = LinearInfo<ParamType>;
};

template <ValidCurveParamType ParamType>
struct DeriveCurveInfoType<CurveType::Bezier, ParamType> {
  using type = BezierInfo<ParamType>;
};

template <ValidCurveParamType ParamType>
struct DeriveCurveInfoType<CurveType::Hermite, ParamType> {
  using type = HermiteInfo<ParamType>;
};

//
//
// -------------------------------------------------------------
// --- CURVE ---------------------------------------------------
// -------------------------------------------------------------
template <CurveType T, ValidCurveParamType ParamType> class Curve {
public:
  Curve() { PC_PRINT("CREATED", TagType::Constr, "Curve") };
  virtual ~Curve() { PC_PRINT("DESTROYED", TagType::Destr, "Curve") };

  virtual float GetValueAt_Fast(float t) = 0;
  virtual double GetValueAt_Slow(float t) = 0;

protected:
  DeriveCurveInfoType<T, ParamType> m_curveInfo;
};

//
//
//
// -------------------------------------------------------------
// --- SPLINE --------------------------------------------------
// -------------------------------------------------------------

template <SplineType V, ValidCurveParamType ParamType> class Spline {
public:
  static constexpr SplineType typeValue = V;

public:
  Spline() { PC_PRINT("CREATED", TagType::Constr, "Spline") };
  ~Spline() { PC_PRINT("DESTROYED", TagType::Destr, "Spline") };

  // -----------------------------------------------------------
  // --- BEZIER ------------------------------------------------

  // -----------------------------------------------------------
  // --- HERMITE -----------------------------------------------

protected:
  float m_globalT = 0.0f; // 'the' parameter. t as in p(t)
  // TODO: Change CurveType based on SplineType
  std::vector<Curve<CurveType::Bezier, ParamType> &> m_knots{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
