#pragma once

#include "Curves.h"
#include "GlobalMacros.h"
#include "Splines.h"
#include <type_traits>
#include <variant>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// --- AnimationPropery stuff  -------------------------------------------------
enum class Axes { X = 0, Y = 1, Z = 2, W = 3 };
template <typename T>
concept IsFloatDoubleInt = std::is_same_v<T, float> || std::is_same_v<T, int> ||
                           std::is_same_v<T, double>;

enum class SelectedCurveSplineType { None = 0, Curve, Spline };

//
// --- AnimationTrack & TimeTrain stuff ----------------------------------------
using CurvePtr =
    std::variant<const Curve<float> *, const Curve<double> *,
                 const Curve<glm::vec2> *, const Curve<glm::vec3> *,
                 const Curve<glm::vec4> *>;
using SplinePtr =
    std::variant<const Spline<float> *, const Spline<double> *,
                 const Spline<glm::vec2> *, const Spline<glm::vec3> *,
                 const Spline<glm::vec4> *>;

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
