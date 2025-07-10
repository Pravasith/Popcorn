#pragma once

#include "Curves/Curves.h"
#include "Curves/CurvesDefs.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <ValidCurveParamType T> struct TimeRail {
  double duration;
  const Curve<T> *curve;
};

enum TimeStationType { Source, Layover, Destination };
template <TimeStationType T> class TimeStation {};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
