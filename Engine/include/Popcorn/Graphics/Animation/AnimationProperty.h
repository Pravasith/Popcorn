#pragma once

#include "AnimationDefs.h"
#include "CurveDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Splines.h"
#include <cassert>
#include <cstddef>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class TimeTrain;

// Owned by classes like GameObject & Material
template <CurveValueType T> class AnimationProperty {
public:
  using value_type = T;

public:
  explicit AnimationProperty(const T &value) : m_value(value) {};

  template <IsFloatDouble... Args>
  explicit AnimationProperty(Args &&...args)
      : m_value(std::forward<Args>(args)...){};

  // template <IsFloatDouble... Args, typename F>
  // explicit AnimationProperty(Args &&...args, F &&afterMorphCb)
  //     : m_value(std::forward<Args>(args)...){};

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

  //
  //
  // --- TRANSFORMATIONS CLASS METHODS ----------------------------------------
  //
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

  // Note: Right now, there is only one single rail binding to each
  // AnimationProperty. In the future make it so we can bind multiple rails for
  // animation blending with weights

  inline T GetValueAt_Fast(float t) const {
    assert(m_railVTable.IsValid() && "Curve/spline not defined");
    return m_railVTable.valueAtFast_Fptr(m_railVTable.rail, t);
  }
  inline T GetValueAt_Slow(double t) const {
    assert(m_railVTable.IsValid() && "Curve/spline not defined");
    return m_railVTable.valueAtSlow_Fptr(m_railVTable.rail, t);
  }
  inline T GetFirstDerivativeAt_Fast(float t) const {
    assert(m_railVTable.IsValid() && "Curve/spline not defined");
    return m_railVTable.velocityAtFast_Fptr(m_railVTable.rail, t);
  }
  inline T GetFirstDerivativeAt_Slow(double t) const {
    assert(m_railVTable.IsValid() && "Curve/spline not defined");
    return m_railVTable.velocityAtSlow_Fptr(m_railVTable.rail, t);
  }

private:
  friend class TimeTrain;

  //
  //
  // --- TIME TRAIN CLASS METHODS ---------------------------------------------
  //
  inline void SetIsAnimating(bool isAnimating) { m_isAnimating = isAnimating; }
  inline void Morph(const T &passengerValue) { m_value = passengerValue; }

  inline void BindRail(const Curve<T> *r) {
    m_railVTable.rail = r;
    m_railVTable.valueAtFast_Fptr = &ValueAtFast_Curve;
    m_railVTable.valueAtSlow_Fptr = &ValueAtSlow_Curve;
    m_railVTable.velocityAtFast_Fptr = &VelocityAtFast_Curve;
    m_railVTable.velocityAtSlow_Fptr = &VelocityAtSlow_Curve;
  }
  inline void BindRail(const Spline<T> *r) {
    m_railVTable.rail = r;
    m_railVTable.valueAtFast_Fptr = &ValueAtFast_Spline;
    m_railVTable.valueAtSlow_Fptr = &ValueAtSlow_Spline;
    m_railVTable.velocityAtFast_Fptr = &VelocityAtFast_Spline;
    m_railVTable.velocityAtSlow_Fptr = &VelocityAtSlow_Spline;
  }

  // thunks for rails (curve & spline)
  static inline T ValueAtFast_Curve(const void *r, float t) noexcept {
    return static_cast<const Curve<T> *>(r)->GetValueAt_Fast(t);
  }
  static inline T ValueAtSlow_Curve(const void *r, double t) noexcept {
    return static_cast<const Curve<T> *>(r)->GetValueAt_Slow(t);
  }
  static inline T VelocityAtFast_Curve(const void *r, float t) noexcept {
    return static_cast<const Curve<T> *>(r)->GetFirstDerivativeAt_Fast(t);
  }
  static inline T VelocityAtSlow_Curve(const void *r, double t) noexcept {
    return static_cast<const Curve<T> *>(r)->GetFirstDerivativeAt_Slow(t);
  }
  static inline T ValueAtFast_Spline(const void *r, float t) noexcept {
    return static_cast<const Spline<T> *>(r)->GetValueAt_Fast(t);
  }
  static inline T ValueAtSlow_Spline(const void *r, double t) noexcept {
    return static_cast<const Spline<T> *>(r)->GetValueAt_Slow(t);
  }
  static inline T VelocityAtFast_Spline(const void *r, float t) noexcept {
    return static_cast<const Spline<T> *>(r)->GetFirstDerivativeAt_Fast(t);
  }
  static inline T VelocityAtSlow_Spline(const void *r, double t) noexcept {
    return static_cast<const Spline<T> *>(r)->GetFirstDerivativeAt_Slow(t);
  }

private:
  template <CurveValueType U> struct RailVTable {
    const void *rail = nullptr;
    U (*valueAtFast_Fptr)(const void *rail, float t) noexcept = nullptr;
    U (*valueAtSlow_Fptr)(const void *rail, double t) noexcept = nullptr;
    U (*velocityAtFast_Fptr)(const void *rail, float t) noexcept = nullptr;
    U (*velocityAtSlow_Fptr)(const void *rail, double t) noexcept = nullptr;
    bool IsValid() const noexcept {
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
