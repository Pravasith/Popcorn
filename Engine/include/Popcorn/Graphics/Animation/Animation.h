#pragma once

#include "AnimationProperty.h"
#include "Event.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
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
    std::variant<AnimationProperty<float> *, AnimationProperty<glm::vec2> *,
                 AnimationProperty<glm::vec3> *,
                 AnimationProperty<glm::vec4> *>;

struct AnimationTrackBase {
  virtual ~AnimationTrackBase() = default;
  virtual void MorphPassengers(double t, bool useSlowAlgo = false) = 0;
};

using AnimationTrackPtr = AnimationTrackBase *;

class TimeTrain {
public:
  TimeTrain(AnimationTrackPtr passengerPtr, double boardStation,
            double destStation);
  TimeTrain(AnimationPropertyPtr passengerPtr, CurvePtr curvePtr,
            double boardStation, double destStation);
  TimeTrain(AnimationPropertyPtr passengerPtr, SplinePtr splinePtr,
            double boardStation, double destStation);
  TimeTrain() = delete;

  inline void Animate_Fast(float u) {
    assert(m.ttExec.animateFast_Fptr);
    m.ttExec.animateFast_Fptr(m.ttExec.psgrPtr, m.ttExec.railPtr, u);
  }
  inline void Animate_Slow(double u) {
    assert(m.ttExec.animateSlow_Fptr);
    m.ttExec.animateSlow_Fptr(m.ttExec.psgrPtr, m.ttExec.railPtr, u);
  }

  void SetIsAnimating(bool a) {
    assert(!m_isPsgrAnimTrack);
    // auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    // (m.ttExec.psgrPtr)->SetIsAnimating(a);
  }

private:
  inline void SetInvLen() {
    const double len = dest - board;
    // Treat tiny spans as instant (avoids 1/very-small blowing up)
    invLen = (len > PC_EPS_D) ? 1.0 / len : 0.0;
  }

  // Animate curve or spline thunks
  template <CurveValueType T>
  static void AnimateFast_Curve(void *animationPropertyPtr,
                                const void *curvePtr, float u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *c = static_cast<const Curve<T> *>(curvePtr);
    p->Morph(c->GetValueAt_Fast(u));
  }
  template <CurveValueType T>
  static void AnimateSlow_Curve(void *animationPropertyPtr,
                                const void *curvePtr, double u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *c = static_cast<const Curve<T> *>(curvePtr);
    p->Morph(c->GetValueAt_Slow(u));
  }
  template <CurveValueType T>
  static void AnimateFast_Spline(void *animationPropertyPtr,
                                 const void *splinePtr, float u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *s = static_cast<const Spline<T> *>(splinePtr);
    p->Morph(s->GetValueAt_Fast(u));
  }
  template <CurveValueType T>
  static void AnimateSlow_Spline(void *animationPropertyPtr,
                                 const void *splinePtr, double u) {
    auto *p = static_cast<AnimationProperty<T> *>(animationPropertyPtr);
    auto *s = static_cast<const Spline<T> *>(splinePtr);
    p->Morph(s->GetValueAt_Slow(u));
  }

  static void AnimateFast_ChildTrack(void *animationTrackPtr, const void *,
                                     float u) {
    auto *t = static_cast<AnimationTrackBase *>(animationTrackPtr);
    t->MorphPassengers(u);
  }
  static void AnimateSlow_ChildTrack(void *animationTrackPtr, const void *,
                                     double u) {
    auto *t = static_cast<AnimationTrackBase *>(animationTrackPtr);
    t->MorphPassengers(u, true);
  }

public:
  double board;
  double dest;
  double invLen;

private:
  AnimationPropertyPtr m_animPropPtr;
  AnimationTrackPtr m_animTrackPtr;

  bool m_isPsgrAnimTrack = false;

  // --- perf stuff -----------------------------------------------------------
  struct TrainExec {
    void *psgrPtr = nullptr;
    const void *railPtr = nullptr;
    void (*animateFast_Fptr)(void *psgrPtr, const void *railPtr,
                             float u) = nullptr;
    void (*animateSlow_Fptr)(void *psgrPtr, const void *railPtr,
                             double u) = nullptr;
  };

  struct {
    TrainExec ttExec;
  } m;
};

class AnimationTrack : public Subscriber, public AnimationTrackBase {
public:
  AnimationTrack(std::vector<TimeTrain> &&timetrains)
      : m_timeTrains(std::move(timetrains)) {
    assert(!m_timeTrains.empty());
    SweepLineSetUp();
  }
  ~AnimationTrack() = default;

  void Insert_Slow(TimeTrain timeTrain) {
    m_timeTrains.push_back(timeTrain);
    SweepLineSetUp();
  }

  virtual void MorphPassengers(double t, bool useSlowAlgo = false) override;

  void Play(double durationInSecs);
  void Play(double durationInSecs,
            std::function<void(AnimationTrack *)> onFinishCb);

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
  // Play stuff (not allowed if this is a parent)
  bool m_isPlaying = false;
  double m_durationS = 0.0;
  double m_elapsedTimeS = 0.0;
  std::function<void(AnimationTrack *)> m_afterPlayCb;

private:
  // For morphing either animProps or childTracks
  std::vector<TimeTrain> m_timeTrains;

  size_t m_startSlider = 0;
  size_t m_endSlider = 0;

  std::vector<uint32_t> m_starts;
  std::vector<uint32_t> m_ends;
  std::vector<uint32_t>
      m_active; // active trains list during a "t" of the current frame
  std::vector<int32_t> m_locInActive; // for swap-deleting the deactivated ones
                                      // from m_active queue
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
