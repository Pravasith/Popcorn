#pragma once

#include "Animation/TimeStations.h"
#include "GlobalMacros.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class AnimationTrack {
public:
  AnimationTrack(const float duration = 500.0f) : m_durationMs(duration) {};
  ~AnimationTrack() = default;

public:
private:
  float m_elapsedTime = 0.0f;
  const float m_durationMs;
  std::vector<float> m_timeStations;

  std::vector<TimeRail<float> *> m_timeRails;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
