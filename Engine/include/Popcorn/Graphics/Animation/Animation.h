#pragma once

#include "Curves.h"
#include "Event.h"
#include "GlobalMacros.h"
#include "Subscriber.h"
#include "TimeEvent.h"
#include <variant>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

using KeyframesCurvePtr =
    std::variant<const Curve<float> *, const Curve<double> *,
                 const Curve<glm::vec2> *, const Curve<glm::vec3> *,
                 const Curve<glm::vec4> *>;

struct TimeTrain {
  KeyframesCurvePtr keyframesCurve;
  float boardStn = 0.0f;
  float destStn = 0.1f;
};

class AnimationTrack : public Subscriber {
public:
  AnimationTrack() = default;
  ~AnimationTrack() = default;

  void Play(double durationInSecs) {
    m_duration = durationInSecs;
    m_isPlaying = true;
  };

private:
  bool OnUpdate(TimeEvent &e) {
    if ((m_elapsedTimeS += e.GetDeltaS()) < m_duration) {
      // TODO: Morph obj props acc. to curve data
    } else {
      m_isPlaying = false;
    }
    return true;
  };

  void OnEvent(Event &e) override {
    if (!m_isPlaying) {
      return;
    }

    EventDispatcher dispatcher{e};
    dispatcher.Dispatch<TimeEvent>(PC_BIND_EVENT_FUNC(TimeEvent, OnUpdate));
  };

public:
private:
  double m_duration = 0.0;
  double m_elapsedTimeS = 0.0;
  std::vector<TimeTrain> m_timeTrains;

  bool m_isPlaying = false;

  // TODO: parent stuff
  std::vector<AnimationTrack *> m_children;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
