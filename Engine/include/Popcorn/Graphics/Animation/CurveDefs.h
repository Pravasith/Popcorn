#pragma once

#include "GlobalMacros.h"
#include <concepts>
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum CurveInfoForms {
  LinearForm,
  HermiteForm,
  BezierForm,
};
//
// enum SplineForm {
//   CatmullRomType,
//   BSplineType,
// };

template <typename U>
concept CurveFormType =
    std::same_as<U, float> || std::same_as<U, double> ||
    std::same_as<U, glm::vec2> || std::same_as<U, glm::vec3> ||
    std::same_as<U, glm::vec4>;

// template <typename T>
// concept ParametericFunctionOutputType =
//     std::copyable<T> && requires(T a, T b, float c) {
//       // a & b 'type' requirements
//       { a + b } -> std::convertible_to<T>;
//       { a - b } -> std::convertible_to<T>;
//       { a * b } -> std::convertible_to<T>;
//     };

template <CurveFormType T> struct CurveInfoLinearForm {
  T p0;
  T p1;
};

template <CurveFormType T> struct CurveInfoBezierForm {
  T b0;
  T b1;
  T b2;
  T b3;
};

template <CurveFormType T> struct CurveInfoHermiteForm {
  T p0;
  T v0;
  T v1;
  T p1;
};

template <CurveInfoForms T, CurveFormType P> struct DeriveCurveInfoType;

template <CurveFormType P>
struct DeriveCurveInfoType<CurveInfoForms::LinearForm, P> {
  using type = CurveInfoLinearForm<P>;
};

template <CurveFormType P>
struct DeriveCurveInfoType<CurveInfoForms::BezierForm, P> {
  using type = CurveInfoBezierForm<P>;
};

template <CurveFormType P>
struct DeriveCurveInfoType<CurveInfoForms::HermiteForm, P> {
  using type = CurveInfoHermiteForm<P>;
};

using CurveHashType = uint64_t;

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
