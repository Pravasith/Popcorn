#pragma once

#include "Animation/CurveDefs.h"
#include "Animation/Splines.h"
#include "GlobalMacros.h"
#include <cstdint>
#include <initializer_list>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <typename U>
concept PassengerType = CurveValueType<U>; // just an alias

template <PassengerType T> class TimeRail {
  TimeRail() {

  };

  std::vector<Knot<T>> keyframe;
  // TODO: With the knots (keyframes), construct Catmull-Rom
  // TODO: Overload with Custom curve/spline option (for Blender-imported
  //       curves, or self created individual curves)
  // TODO: Create JSON-Popcorn spline converter

  uint32_t boardingStation;    // 0 -> 1
  uint32_t destinationStation; // 0 -> 1
};

struct TimeTrain {
  // std::vector<TimeRail> timeRails;
  // std::vector<Passengers> passengers;
  // std::vector<const Curve>
};

class AnimationTrack {
public:
  AnimationTrack(const float duration = 500.0f,
                 std::initializer_list<float> timeStations = {})
      : m_durationMs(duration), m_timeStations(timeStations) {};

  ~AnimationTrack() = default;

public:
private:
  float m_elapsedT = 0.0f;
  const float m_durationMs;
  std::vector<float> m_timeStations;
  // std::vector<TimeRail> m_timeRails;
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
