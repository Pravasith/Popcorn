#pragma once

#include "Animation/TimeStations.h"
#include "GlobalMacros.h"
#include <initializer_list>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class AnimationTrack {
public:
  AnimationTrack(const float duration = 500.0f,
                 std::initializer_list<float> timeStations = {})
      : m_durationMs(duration), m_timeStations(timeStations) {
          //
          // TODO: Create a hashmap for all curve instances, except the hash
          //       value is custom (hash mixing - paramType value, curveInfo
          //       values)
          //
          // TODO: Asserts for timeStations (sorted)
          // TODO: Asserts for duration (>0)
          // TODO: Asserts for timeRails
          //
        };

  ~AnimationTrack() = default;

public:
private:
  float m_elapsedT = 0.0f;
  const float m_durationMs;
  std::vector<float> m_timeStations;
  std::vector<TimeRail> m_timeRails;
};

// void AddTimeRails() {
//   // TODO: Check for duplicates in m_timeStations
//   // TODO: Sort and add to m_timeStations
// };

// template <ValidCurveParamType T>
// void AddTimeRail(const TimeRail<T> *timeRailPtr) {
//   m_timeRails.emplace_back(timeRailPtr);
// };

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
