#pragma once

#include "GlobalMacros.h"
#include <concepts>
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <typename U>
concept CurveValueType =
    std::same_as<U, float> || std::same_as<U, double> ||
    std::same_as<U, glm::vec2> || std::same_as<U, glm::vec3> ||
    std::same_as<U, glm::vec4>;

template <CurveValueType T> struct CurveInfoLinearForm {
  T p0;
  T p1;
};

template <CurveValueType T> struct CurveInfoBezierForm {
  T b0;
  T b1;
  T b2;
  T b3;
};

template <CurveValueType T> struct CurveInfoHermiteForm {
  T p0;
  T v0;
  T v1;
  T p1;
};

using CurveHashType = uint64_t;

template <CurveValueType T> struct HermiteKnot {
  T vIn;
  T val;
  T vOut;
  double t; // [0, 1)
};

template <CurveValueType T> struct SimpleKnot {
  T val;
  double t; // [0, 1)
};

// For splines made of cubic hermite curves
template <CurveValueType T> struct SplineInfoHermiteForm {
  std::vector<HermiteKnot<T>> knots;
};

// For splines made of linear curves
template <CurveValueType T> struct SplineInfoSimpleForm {
  std::vector<SimpleKnot<T>> knots;
};

// For physical representation of curves (ex. camera rails)
template <CurveValueType T> struct SplineInfoBezierForm {
  std::vector<CurveInfoBezierForm<T>> bezierCurveInfoValues;
  std::vector<double> tValues; // 't' or 'u' values of p(t) or p(u)
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
