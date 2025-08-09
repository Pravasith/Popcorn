#pragma once

#include "Animation/CurveDefs.h"
#include "Animation/Splines.h"
#include "GlobalMacros.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <CurveValueType T> class TimeRail {
  TimeRail();
  ~TimeRail();

  std::vector<Knot<T>> keyframe;

  // TODO: With the knots (keyframes), construct Catmull-Rom
  // TODO: Overload with Custom curve/spline option (for Blender-imported
  //       curves, or self created individual curves)
  // TODO: Create JSON-Popcorn spline converter
};

// class AnimationTrack {
// public:
//   AnimationTrack(float durationMs = 500.0f, std::vector<TimeTrain>
//   &timeTrains)
//       : m_durationMs(durationMs), m_timeTrains(timeTrains) {};
//   ~AnimationTrack() = default;
//
// public:
// private:
//   float m_elapsedT = 0.0f;
//   const float m_durationMs;
//   std::vector<float> m_timeRails;
// };

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
