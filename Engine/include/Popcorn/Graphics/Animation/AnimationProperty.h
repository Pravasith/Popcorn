#pragma once

#include "CurveDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Splines.h"
#include <cassert>
#include <type_traits>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class Axes { X = 0, Y = 1, Z = 2, W = 3 };
template <typename T>
concept IsFloatDoubleInt = std::is_same_v<T, float> || std::is_same_v<T, int> ||
                           std::is_same_v<T, double>;

enum class SelectedCurveSplineType { None = 0, Curve, Spline };

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

  T GetValueAt_Fast(float t) const {
    assert((m_curvePtr || m_splinePtr) && "Curve/spline not defined");
    if (m_splinePtr)
      return m_splinePtr->GetValueAt_Fast(t);
    return m_curvePtr->GetValueAt_Fast(t);
  }
  T GetValueAt_Slow(double t) const {
    assert((m_curvePtr || m_splinePtr) && "Curve/spline not defined");
    if (m_splinePtr)
      return m_splinePtr->GetValueAt_Slow(t);
    return m_curvePtr->GetValueAt_Slow(t);
  }
  T GetFirstDerivativeAt_Fast(float t) const {
    assert((m_curvePtr || m_splinePtr) && "Curve/spline not defined");
    if (m_splinePtr)
      return m_splinePtr->GetFirstDerivativeAt_Fast(t);
    return m_curvePtr->GetFirstDerivativeAt_Fast(t);
  }
  T GetFirstDerivativeAt_Slow(double t) const {
    assert((m_curvePtr || m_splinePtr) && "Curve/spline not defined");
    if (m_splinePtr)
      return m_splinePtr->GetFirstDerivativeAt_Slow(t);
    return m_curvePtr->GetFirstDerivativeAt_Slow(t);
  }

public:
  void SetCurvePtr(const Curve<T> *c) {
    m_curvePtr = c;
    m_splinePtr = nullptr;
  }
  void SetSplinePtr(const Spline<T> *s) {
    m_splinePtr = s;
    m_curvePtr = nullptr;
  }

private:
  const Curve<T> *m_curvePtr = nullptr;
  const Spline<T> *m_splinePtr = nullptr;

  void SetIsAnimating(bool isAnimating) { m_isAnimating = isAnimating; }

private:
  bool m_isAnimating = false;
  T m_value;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
