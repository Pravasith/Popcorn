#pragma once

#include "Animation/TimeStations.h"
#include "Curves/CurvesDefs.h"
#include "GlobalMacros.h"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <initializer_list>
#include <unordered_map>
#include <variant>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

using TimeRailPtrType =
    std::variant<const TimeRail<float> *, const TimeRail<double> *,
                 const TimeRail<glm::vec3> *, const TimeRail<glm::vec2> *
                 // TODO: Add more
                 >;

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
