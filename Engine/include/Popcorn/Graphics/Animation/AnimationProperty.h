#pragma once

#include "CurveDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Splines.h"
#include <cassert>
#include <type_traits>
#include <variant>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class Axes { X = 0, Y = 1, Z = 2, W = 3 };
template <typename T>
concept IsFloatDoubleInt = std::is_same_v<T, float> || std::is_same_v<T, int> ||
                           std::is_same_v<T, double>;

// Owned by classes like GameObject & Material
template <CurveFormType T> class AnimationProperty {
public:
  using value_type = T;

public:
  explicit AnimationProperty(const T &value) : m_value(value) {};
  template <IsFloatDoubleInt... Args>
  explicit AnimationProperty(Args &&...args)
      : m_value(std::forward<Args>(args)...){};

  [[nodiscard]] const T &GetValue() const { return m_value; };

#define BLOCK_IF_ANIMATING_ALREADY                                             \
  do {                                                                         \
    if (m_isAnimating) {                                                       \
      PC_WARN(                                                                 \
          "Operation not performed because an animation is already running "   \
          "on this property")                                                  \
      return;                                                                  \
    }                                                                          \
  } while (0);

  void Set(const T &value) noexcept {
    BLOCK_IF_ANIMATING_ALREADY
    m_value = value;
  }
  template <typename F> void Set(const T &value, F &&onSet) {
    BLOCK_IF_ANIMATING_ALREADY
    m_value = value;
    std::forward<F>(onSet)();
  }

  template <Axes A> void AddComponent(float value) noexcept {
    static_assert(!std::is_same_v<T, float> && !std::is_same_v<T, double> &&
                  "Cannot add component on a scalar value");
    constexpr size_t I = (size_t)A;
    static_assert(I < T::length(), "component index out of range for this T");
    BLOCK_IF_ANIMATING_ALREADY
    m_value[I] += value;
  }
  template <Axes A, typename F> void AddComponent(float value, F &&afterAdd) {
    static_assert(!std::is_same_v<T, float> && !std::is_same_v<T, double> &&
                  "Cannot add component on a scalar value");
    constexpr size_t I = (size_t)A;
    static_assert(I < T::length(), "component index out of range for this T");
    BLOCK_IF_ANIMATING_ALREADY
    m_value[I] += value;
    std::forward<F>(afterAdd)();
  }

  template <Axes A> void MultiplyComponent(float value) noexcept {
    static_assert(!std::is_same_v<T, float> && !std::is_same_v<T, double> &&
                  "Cannot multiply component on a scalar value");
    constexpr size_t I = (size_t)A;
    static_assert(I < T::length(), "component index out of range for this T");
    BLOCK_IF_ANIMATING_ALREADY
    m_value[I] *= value;
  }
  template <Axes A, typename F>
  void MultiplyComponent(float value, F &&afterMultiply) {
    static_assert(!std::is_same_v<T, float> && !std::is_same_v<T, double> &&
                  "Cannot multiply component on a scalar value");
    constexpr size_t I = (size_t)A;
    static_assert(I < T::length(), "component index out of range for this T");
    BLOCK_IF_ANIMATING_ALREADY
    m_value[I] *= value;
    std::forward<F>(afterMultiply)();
  }
#undef BLOCK_IF_ANIMATING_ALREADY

  T GetInstValue() const { return m_value; }
  T GetValueAtAt_Slow(double t) const {
    assert(m_curveOrSplinePtr && "Curve/spline not defined");

    return std::visit(
        [&t](const auto &curveOrSpline) {
          return curveOrSpline->GetValueAtAt_Slow(t);
        },
        m_curveOrSplinePtr);
  }
  T GetValueAtAt_Fast(float t) const {
    assert(m_curveOrSplinePtr && "Curve/spline not defined");
    return std::visit(
        [&t](const auto &curveOrSpline) {
          return curveOrSpline->GetValueAtAt_Fast(t);
        },
        m_curveOrSplinePtr);
  }
  T GetFirstDerivativeAt_Slow(double t) const {
    assert(m_curveOrSplinePtr && "Curve/spline not defined");
    return std::visit(
        [&t](const auto &curveOrSpline) {
          return curveOrSpline->GetFirstDerivativeAt_Slow(t);
        },
        m_curveOrSplinePtr);
  }
  T GetFirstDerivativeAt_Fast(float t) const {
    assert(m_curveOrSplinePtr && "Curve/spline not defined");
    return std::visit(
        [&t](const auto &curveOrSpline) {
          return curveOrSpline->GetFirstDerivativeAt_Fast(t);
        },
        m_curveOrSplinePtr);
  }

public:
  using CurveOrSplinePtr = std::variant<const Curve<T> *, const Spline<T> *>;
  void SetCurvePtr(const Curve<T> *curvePtr) { m_curveOrSplinePtr = curvePtr; }
  void SetSplinePtr(const Spline<T> *splinePtr) {
    m_curveOrSplinePtr = splinePtr;
  }

private:
  CurveOrSplinePtr m_curveOrSplinePtr;
  void SetIsAnimating(bool isAnimating) { m_isAnimating = isAnimating; }

private:
  bool m_isAnimating = false;
  T m_value;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
