#pragma once

#include "Curves.h"
#include "Event.h"
#include "GameObject.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include "Subscriber.h"
#include "TimeEvent.h"
#include <functional>
#include <variant>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

using KeyframesCurvePtr =
    std::variant<const Curve<float> *, const Curve<double> *,
                 const Curve<glm::vec2> *, const Curve<glm::vec3> *,
                 const Curve<glm::vec4> *>;

struct TimeTrain {
  const GameObject *passenger;
  KeyframesCurvePtr keyframesCurve;
  float boardStn = 0.0f;
  float destStn = 0.1f;
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

  void OnEvent(Event &e) override {
    if (!m_isPlaying) {
      return;
    }

    EventDispatcher dispatcher{e};
    dispatcher.Dispatch<TimeEvent>(PC_BIND_EVENT_FUNC(TimeEvent, OnUpdate));
  };

private:
#define RESET_PROPS                                                            \
  m_isPlaying = false;                                                         \
  m_elapsedTimeS = 0.0;                                                        \
  m_durationS = 0.0;                                                           \
  m_onPlayFinishCb = nullptr;

  bool OnUpdate(TimeEvent &e) {
    if ((m_elapsedTimeS += e.GetDeltaS()) <= m_durationS) {
      // TODO: Morph obj props acc. to curve data

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
  double m_durationS = 0.0;
  double m_elapsedTimeS = 0.0;
  const std::function<void(AnimationTrack *)> *m_onPlayFinishCb = nullptr;
  bool m_isPlaying = false;

private:
  std::vector<TimeTrain> m_timeTrains;

  // TODO: parent stuff
  std::vector<AnimationTrack *> m_children;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
