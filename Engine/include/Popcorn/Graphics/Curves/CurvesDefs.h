#pragma once

#include "GlobalMacros.h"
#include <concepts>

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

template <CurveType T, ValidCurveParamType P> struct DeriveCurveInfoType;

template <ValidCurveParamType P>
struct DeriveCurveInfoType<CurveType::Linear, P> {
  using type = LinearInfo<P>;
};

template <ValidCurveParamType P>
struct DeriveCurveInfoType<CurveType::Bezier, P> {
  using type = BezierInfo<P>;
};

template <ValidCurveParamType P>
struct DeriveCurveInfoType<CurveType::Hermite, P> {
  using type = HermiteInfo<P>;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
