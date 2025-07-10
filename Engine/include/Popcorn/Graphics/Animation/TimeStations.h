#pragma once

#include "Curves/Curves.h"
#include "Curves/CurvesDefs.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <ValidCurveParamType T> struct TimeRail {
  const Curve<T> *curve = nullptr;
  float connectingStations[2]{
      0.0f, 1.0f}; // Array value can be anything [0, 1].
                   // Relative to the time in the AnimationTrack
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
