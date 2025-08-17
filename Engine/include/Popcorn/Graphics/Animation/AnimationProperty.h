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

class TimeTrain;

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

  inline T GetValueAt_Fast(float t) const {
    assert(m_railVTable.IsValid() && "Curve/spline not defined");
    return (*m_railVTable.valueAtFast_Fptr)(t);
  }
  inline T GetValueAt_Slow(double t) const {
    assert(m_railVTable.IsValid() && "Curve/spline not defined");
    return (*m_railVTable.valueAtSlow_Fptr)(t);
  }
  inline T GetFirstDerivativeAt_Fast(float t) const {
    assert(m_railVTable.IsValid() && "Curve/spline not defined");
    return (*m_railVTable.velocityAtFast_Fptr)(t);
  }
  inline T GetFirstDerivativeAt_Slow(double t) const {
    assert(m_railVTable.IsValid() && "Curve/spline not defined");
    return (*m_railVTable.velocityAtSlow_Fptr)(t);
  }

private:
  friend class TimeTrain;

  inline void SetIsAnimating(bool isAnimating) { m_isAnimating = isAnimating; }
  inline void Morph(const T &passengerValue) { m_value = passengerValue; }

#define BIND_RAIL_STUFF                                                        \
  m_railVTable.rail = r;                                                       \
  m_railVTable.valueAtFast_Fptr = &r->GetValueAt_Fast;                         \
  m_railVTable.valueAtSlow_Fptr = &r->GetValueAt_Slow;                         \
  m_railVTable.velocityAtFast_Fptr = &r->GetFirstDerivativeAt_Fast;            \
  m_railVTable.velocityAtSlow_Fptr = &r->GetFirstDerivativeAt_Slow;

  inline void BindRail(const Curve<T> *r) { BIND_RAIL_STUFF }
  inline void BindRail(const Spline<T> *r) { BIND_RAIL_STUFF }
#undef BIND_RAIL_STUFF

private:
  template <CurveFormType U> struct RailVTable {
    void *rail = nullptr;
    U (*valueAtFast_Fptr)(float t) = nullptr;
    U (*valueAtSlow_Fptr)(float t) = nullptr;
    U (*velocityAtFast_Fptr)(float t) = nullptr;
    U (*velocityAtSlow_Fptr)(float t) = nullptr;
    bool IsValid() {
      return rail && valueAtSlow_Fptr && valueAtFast_Fptr &&
             velocityAtSlow_Fptr && velocityAtFast_Fptr;
    }
  };

  RailVTable<T> m_railVTable{};

private:
  bool m_isAnimating = false;
  T m_value;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
