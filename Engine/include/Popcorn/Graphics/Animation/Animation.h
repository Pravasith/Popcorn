#pragma once

#include "CurveDefs.h"
#include "Curves.h"
#include "Event.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include "Subscriber.h"
#include "TimeEvent.h"
#include <functional>
#include <glm/ext/vector_float2.hpp>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

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

  void Set(const T &value) noexcept { m_value = value; };
  template <typename F> void Set(const T &value, F &&onSet) noexcept {
    m_value = value;
    std::forward<F>(onSet)();
  };

  template <std::size_t I> void AddComponent(float value) noexcept {
    static_assert(!std::is_same_v<T, float> && !std::is_same_v<T, double> &&
                  "Cannot add component on a scalar value");
    static_assert(I < T::length(), "component index out of range for this T");
    m_value[I] += value;
  }
  template <std::size_t I, typename F>
  void AddComponent(float value, F &&onAdd) noexcept {
    static_assert(!std::is_same_v<T, float> && !std::is_same_v<T, double> &&
                  "Cannot add component on a scalar value");
    static_assert(I < T::length(), "component index out of range for this T");
    m_value[I] += value;
    std::forward<F>(onAdd)();
  }

private:
  bool m_isAnimating = false;
  T m_value;
};

template <CurveValueType T> struct CurveBinding {
  AnimationProperty<T> *animatable;
  const Curve<T> *keysCurve;
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
  AnimationTrack() = default;
  ~AnimationTrack() = default;

  void Play(double durationInSecs) {
    m_durationS = durationInSecs;
    m_isPlaying = true;
  };

  void Play(double durationInSecs,
            const std::function<void(AnimationTrack *)> &onFinishCb) {
    m_durationS = durationInSecs;
    m_isPlaying = true;
    m_onPlayFinishCb = &onFinishCb;
  };

  void Play(double durationInSecs,
            const std::function<void(AnimationTrack *)> &onFinishCb,
            const std::function<void(AnimationTrack *)> &inProgressCb) {
    m_durationS = durationInSecs;
    m_isPlaying = true;
    m_onPlayFinishCb = &onFinishCb;
    m_inPlayProgressCb = &inProgressCb;
  };

  void OnEvent(Event &e) override {
    if (!m_isPlaying) {
      return;
    }
    EventDispatcher dispatcher{e};
    dispatcher.Dispatch<TimeEvent>(PC_BIND_EVENT_FUNC(TimeEvent, OnUpdate));
  };

private:
#define RESET_PROPS                                                            \
  do {                                                                         \
    m_isPlaying = false;                                                       \
    m_elapsedTimeS = 0.0;                                                      \
    m_durationS = 0.0;                                                         \
    m_onPlayFinishCb = nullptr;                                                \
  } while (0);

  bool OnUpdate(TimeEvent &e) {
    if ((m_elapsedTimeS += e.GetDeltaS()) <= m_durationS) {
      // TODO: Morph obj props acc. to curve data
      for (const TimeTrain &tt : m_timeTrains) {
        // TODO:
        // auto &animatable = tt.timetrain->curveBinding.animatable;
        // animatable->Animate<X>(
        //      tt.keysCurve->ValueAt(m_elapsedTimeS));
        //
        //
      }

      if (m_inPlayProgressCb) {
        (*m_inPlayProgressCb)(this);
      }

      if (m_durationS - m_elapsedTimeS < PC_EPS) {
        // done playing
        if (m_onPlayFinishCb) {
          (*m_onPlayFinishCb)(this);
        }
        RESET_PROPS
      }
    } else {
      // just in-case the epsilon code block above gets skipped
      RESET_PROPS
    }

    return true;
  };

public:
private:
  bool m_isPlaying = false;
  double m_durationS = 0.0;
  double m_elapsedTimeS = 0.0;
  const std::function<void(AnimationTrack *)> *m_onPlayFinishCb = nullptr;
  const std::function<void(AnimationTrack *)> *m_inPlayProgressCb = nullptr;

private:
  std::vector<TimeTrain> m_timeTrains;

  // TODO: parent stuff
  std::vector<AnimationTrack *> m_children;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
