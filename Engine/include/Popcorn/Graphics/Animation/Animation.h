#pragma once

#include "AnimationProperty.h"
#include "CurveDefs.h"
#include "Curves.h"
#include "Event.h"
#include "GlobalMacros.h"
#include "Splines.h"
#include "Subscriber.h"
#include "TimeEvent.h"
#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

using AnimationPropertyPtr =
    std::variant<AnimationProperty<float> *, AnimationProperty<double> *,
                 AnimationProperty<glm::vec2> *, AnimationProperty<glm::vec3> *,
                 AnimationProperty<glm::vec4> *>;

using CurvePtr =
    std::variant<const Curve<float> *, const Curve<double> *,
                 const Curve<glm::vec2> *, const Curve<glm::vec3> *,
                 const Curve<glm::vec4> *>;
using SplinePtr =
    std::variant<const Spline<float> *, const Spline<double> *,
                 const Spline<glm::vec2> *, const Spline<glm::vec3> *,
                 const Spline<glm::vec4> *>;

class TimeTrain {
  friend class AnimationTrack;

  TimeTrain(AnimationPropertyPtr passengerPtr, CurvePtr curvePtr,
            float boardStation, float destStation) {

    std::visit(
        [&](auto *curvePtrVal) {
          std::visit(
              [&](auto *passengerPtrVal) {
                using PassengerValueType = typename std::decay_t<
                    decltype(*passengerPtrVal)>::value_type;
                using CurveValueType =
                    typename std::decay_t<decltype(*curvePtrVal)>::value_type;

                if constexpr (std::is_same_v<PassengerValueType,
                                             CurveValueType>) {
                  passengerPtrVal->BindRail(curvePtrVal);

                  m.trainExec.animPropPtr = passengerPtrVal;
                  m.trainExec.crvSplPtr = curvePtrVal;
                  m.trainExec.animateFast_Fptr =
                      &AnimateFast_Curve<CurveValueType>;
                  m.trainExec.animateSlow_Fptr =
                      &AnimateSlow_Curve<CurveValueType>;

                  m_passengerPtr = passengerPtr;
                  m_boardStation = boardStation;
                  m_destStation = destStation;
                } else {
                  throw std::runtime_error(
                      "trying to bind a Curve to an AnimationProperty that are "
                      "of different types");
                }
              },
              passengerPtr);
        },
        curvePtr);
  }
  TimeTrain(AnimationPropertyPtr passengerPtr, SplinePtr splinePtr,
            float boardStation, float destStation) {
    std::visit(
        [&](auto *splinePtrVal) {
          std::visit(
              [&](auto *passengerPtrVal) {
                using PassengerValueType = typename std::decay_t<
                    decltype(*passengerPtrVal)>::value_type;
                using SplineValueType =
                    typename std::decay_t<decltype(*splinePtrVal)>::value_type;
                if constexpr (std::is_same_v<PassengerValueType,
                                             SplineValueType>) {
                  passengerPtrVal->BindRail(splinePtrVal);

                  m.trainExec.animPropPtr = passengerPtrVal;
                  m.trainExec.crvSplPtr = splinePtrVal;
                  m.trainExec.animateFast_Fptr =
                      &AnimateFast_Spline<SplineValueType>;
                  m.trainExec.animateSlow_Fptr =
                      &AnimateSlow_Spline<SplineValueType>;

                  m_passengerPtr = passengerPtr;
                  m_boardStation = boardStation;
                  m_destStation = destStation;
                } else {
                  throw std::runtime_error("trying to bind a Spline to an "
                                           "AnimationProperty that are "
                                           "of different types");
                }
              },
              passengerPtr);
        },
        splinePtr);
  }
  TimeTrain() = delete;

  // Animate curve or spline thunks
  template <CurveFormType T>
  static void AnimateFast_Curve(void *animationPropertyPtr,
                                const void *curvePtr, float u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *c = static_cast<const Curve<T> *>(curvePtr);
    p->Morph(c->GetValueAt_Fast(u));
  };
  template <CurveFormType T>
  static void AnimateSlow_Curve(void *animationPropertyPtr,
                                const void *curvePtr, double u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *c = static_cast<const Curve<T> *>(curvePtr);
    p->Morph(c->GetValueAt_Slow(u));
  };
  template <CurveFormType T>
  static void AnimateFast_Spline(void *animationPropertyPtr,
                                 const void *splinePtr, float u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *s = static_cast<const Spline<T> *>(splinePtr);
    p->Morph(s->GetValueAt_Fast(u));
  };
  template <CurveFormType T>
  static void AnimateSlow_Spline(void *animationPropertyPtr,
                                 const void *splinePtr, double u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *s = static_cast<const Spline<T> *>(splinePtr);
    p->Morph(s->GetValueAt_Slow(u));
  };

private:
  AnimationPropertyPtr m_passengerPtr;
  float m_boardStation = 0.0f;
  float m_destStation = 1.0f;

  // --- perf stuff -----------------------------------------------------------
  struct TrainExec {
    void *animPropPtr = nullptr;
    const void *crvSplPtr = nullptr;
    void (*animateFast_Fptr)(void *animationPropertyPtr,
                             const void *curveOrSplinePtr, float u) = nullptr;
    void (*animateSlow_Fptr)(void *animationPropertyPtr,
                             const void *curveOrSplinePtr, double u) = nullptr;
  };

  struct {
    TrainExec trainExec;
  } m;
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
      if (a.m_boardStation != b.m_boardStation)
        return a.m_boardStation < b.m_boardStation;
      else
        return a.m_destStation < b.m_destStation;
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
                if (a.m_boardStation < b.m_boardStation)
                  return true;
                else if (a.m_boardStation > b.m_boardStation)
                  return false;
                else
                  return a.m_destStation < b.m_destStation;
              });
  }

  inline double GetNormalizedElapsedSecs() const {
    assert(m_durationS > 0);
    return std::clamp(m_elapsedTimeS / m_durationS, 0.0, 1.0);
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
        auto &board = tt.m_boardStation;
        auto &dest = tt.m_destStation;

        double t = GetNormalizedElapsedSecs();
        // TODO:
        // animation stuff
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
