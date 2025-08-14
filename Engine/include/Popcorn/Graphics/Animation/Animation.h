#pragma once

#include "AnimationProperty.h"
#include "Event.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include "Subscriber.h"
#include "TimeEvent.h"
#include <algorithm>
#include <cstddef>
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
    std::variant<Curve<float> *, Curve<double> *, Curve<glm::vec2> *,
                 Curve<glm::vec3> *, Curve<glm::vec4> *>;

using SplinePtr =
    std::variant<Spline<float> *, Spline<double> *, Spline<glm::vec2> *,
                 Spline<glm::vec3> *, Spline<glm::vec4> *>;

struct TimeTrain {
  AnimationPropertyPtr passenger;
  float boardStn = 0.0f;
  float destStn = 1.0f;
  TimeTrain(CurvePtr) {}
  TimeTrain(SplinePtr) {}
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

        // TODO:
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
