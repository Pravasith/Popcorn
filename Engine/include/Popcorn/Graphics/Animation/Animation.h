#pragma once

#include "CurveDefs.h"
#include "Curves.h"
#include "Event.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include "Splines.h"
#include "Subscriber.h"
#include "TimeEvent.h"
#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class Axes { X = 0, Y = 1, Z = 2, W = 3 };
template <typename T>
concept IsFloatDoubleInt = std::is_same_v<T, float> || std::is_same_v<T, int> ||
                           std::is_same_v<T, double>;

// Owned by classes like GameObject & Material
template <CurveValueType T> class AnimationProperty {
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

private:
  void SetIsAnimating(bool isAnimating) { m_isAnimating = isAnimating; }

private:
  bool m_isAnimating = false;
  T m_value;
};

template <CurveValueType T> struct CurveBinding {
  AnimationProperty<T> *animationProperty;
  std::variant<const Curve<T> *, const Spline<T> *> curveOrSpline;
};

using CurveBindingVariant =
    std::variant<CurveBinding<float>, CurveBinding<double>,
                 CurveBinding<glm::vec2>, CurveBinding<glm::vec3>,
                 CurveBinding<glm::vec4>>;

struct TimeTrain {
  CurveBindingVariant curveBinding;
  float boardStn = 0.0f;
  float destStn = 1.0f;
};

class AnimationTrack : public Subscriber {
public:
  AnimationTrack(std::vector<TimeTrain> &&timetrains)
      : m_timeTrains(std::move(timetrains)) {
    Sort();
  }
  ~AnimationTrack() = default;

  void Insert(TimeTrain timeTrain) {
    // list is already sorted at this point
    auto cmp = [](const TimeTrain &a, const TimeTrain &b) {
      if (a.boardStn != b.boardStn)
        return a.boardStn < b.boardStn;
      else
        return a.destStn < b.destStn;
    };
    auto pos = std::upper_bound(m_timeTrains.begin(), m_timeTrains.end(),
                                timeTrain, cmp);
    m_timeTrains.insert(pos, timeTrain);
  }

  void Play(double durationInSecs) {
    m_durationS = durationInSecs;
    m_isPlaying = true;
  }

  void Play(double durationInSecs,
            std::function<void(AnimationTrack *)> onFinishCb) {
    m_durationS = durationInSecs;
    m_isPlaying = true;
    m_onPlayFinishCb = std::move(onFinishCb);
  }

  void OnEvent(Event &e) override {
    if (!m_isPlaying) {
      return;
    }
    EventDispatcher dispatcher{e};
    dispatcher.Dispatch<TimeEvent>(PC_BIND_EVENT_FUNC(TimeEvent, OnUpdate));
  }

private:
  void Sort() {
    std::sort(m_timeTrains.begin(), m_timeTrains.end(),
              [](const TimeTrain &a, const TimeTrain &b) {
                if (a.boardStn < b.boardStn)
                  return true;
                else if (a.boardStn > b.boardStn)
                  return false;
                else
                  return a.destStn < b.destStn;
              });
  }

  inline double GetNormalizedElapsedSecs() const {
    assert(m_durationS > 0);
    if (m_elapsedTimeS - m_durationS < PC_EPS) {
      return 1.0;
    }

    return m_elapsedTimeS / m_durationS;
  }

private:
#define RESET_PROPS                                                            \
  do {                                                                         \
    m_isPlaying = false;                                                       \
    m_elapsedTimeS = 0.0;                                                      \
    m_durationS = 0.0;                                                         \
    m_onPlayFinishCb = nullptr;                                                \
  } while (0);

  bool OnUpdate(TimeEvent &e) {
    if ((m_elapsedTimeS += e.GetDeltaS()) < m_durationS) {
      for (const TimeTrain &tt : m_timeTrains) {
        auto &board = tt.boardStn;
        auto &dest = tt.destStn;

        double t = GetNormalizedElapsedSecs();

        if (t < board)
          return true;
        if (t > dest)
          continue;

        if (t >= board && t <= dest) {
          // TODO: Animate
        }

        // TODO:
        //
        // auto &animatable = tt.timetrain->curveBinding.animatable;
        // animatable->Animate<X>(
        //      tt.keysCurve->ValueAt(m_elapsedTimeS));
      }
    } else {
      if (m_onPlayFinishCb) {
        (m_onPlayFinishCb)(this);
      }
      RESET_PROPS
    }
    return true;
  };
#undef RESET_PROPS

private:
  bool m_isPlaying = false;
  double m_durationS = 0.0;
  double m_elapsedTimeS = 0.0;

  std::function<void(AnimationTrack *)> m_onPlayFinishCb;

private:
  std::vector<TimeTrain> m_timeTrains;

  // TODO: parent stuff
  std::vector<AnimationTrack *> m_children;
  std::unordered_map<float, size_t> m_boardStnIndicesMap;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
