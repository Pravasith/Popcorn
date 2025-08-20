#pragma once

#include "AnimationProperty.h"
#include "CurveDefs.h"
#include "Curves.h"
#include "Event.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include "Splines.h"
#include "Subscriber.h"
#include "TimeEvent.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
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
public:
  TimeTrain(AnimationPropertyPtr passengerPtr, CurvePtr curvePtr,
            double boardStation, double destStation);
  TimeTrain(AnimationPropertyPtr passengerPtr, SplinePtr splinePtr,
            double boardStation, double destStation);
  TimeTrain() = delete;

  inline void AnimateFast(float u) {
    m.ttExec.animateFast_Fptr(m.ttExec.animPropPtr, m.ttExec.crvSplPtr, u);
  }
  inline void AnimateSlow(double u) {
    m.ttExec.animateSlow_Fptr(m.ttExec.animPropPtr, m.ttExec.crvSplPtr, u);
  }

private:
  inline void SetInvLen() {
    const double len = dest - board;
    // Treat tiny spans as instant (avoids 1/very-small blowing up)
    invLen = (len > PC_EPS_D) ? 1.0 / len : 0.0;
  }

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

public:
  double board;
  double dest;
  double invLen;

private:
  AnimationPropertyPtr m_passengerPtr;

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
    TrainExec ttExec;
  } m;
};

class AnimationTrack : public Subscriber {
public:
  AnimationTrack(std::vector<TimeTrain> &&timetrains)
      : m_timeTrains(std::move(timetrains)) {
    assert(!m_timeTrains.empty());
    SweepLineSetUp();
  }
  ~AnimationTrack() = default;

  void Insert(TimeTrain timeTrain) {
    m_timeTrains.push_back(timeTrain);
    SweepLineSetUp();
  }

#define RESET_SWEEP_LINE_SETS                                                  \
  do {                                                                         \
    m_startSlider = m_endSlider = 0;                                           \
    m_active.clear();                                                          \
    std::fill(m_locInActive.begin(), m_locInActive.end(), -1);                 \
  } while (0);

  void Play(double durationInSecs) {
    m_elapsedTimeS = 0.0;
    m_durationS = durationInSecs;
    m_isPlaying = true;
    RESET_SWEEP_LINE_SETS
  }

  void Play(double durationInSecs,
            std::function<void(AnimationTrack *)> onFinishCb) {
    m_elapsedTimeS = 0.0;
    m_durationS = durationInSecs;
    m_isPlaying = true;
    m_onPlayFinishCb = std::move(onFinishCb);
    RESET_SWEEP_LINE_SETS
  }
#undef RESET_SWEEP_LINE_SETS

  void OnEvent(Event &e) override {
    if (!m_isPlaying) {
      return;
    }
    EventDispatcher dispatcher{e};
    dispatcher.Dispatch<TimeEvent>(PC_BIND_EVENT_FUNC(TimeEvent, OnUpdate));
  }

private:
  void SweepLineSetUp();
  inline double GetNormalizedElapsedSecs() const {
    assert(m_durationS > 0);
    return std::clamp(m_elapsedTimeS / m_durationS, 0.0, 1.0);
  }

private:
  bool OnUpdate(TimeEvent &e);

private:
  bool m_isPlaying = false;
  double m_durationS = 0.0;
  double m_elapsedTimeS = 0.0;

  std::function<void(AnimationTrack *)> m_onPlayFinishCb;

private:
  std::vector<TimeTrain> m_timeTrains;

  size_t m_startSlider = 0;
  size_t m_endSlider = 0;

  std::vector<uint32_t> m_starts;
  std::vector<uint32_t> m_ends;
  std::vector<uint32_t>
      m_active; // active trains list during a "t" of the current frame
  std::vector<int32_t> m_locInActive; // for swap-deleting the deactivated ones
                                      // from m_active queue

  // TODO: parent stuff
  std::vector<AnimationTrack *> m_children;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
