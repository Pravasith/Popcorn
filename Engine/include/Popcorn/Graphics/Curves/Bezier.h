#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum BezierCurveType { Static, Dynamic };

template <BezierCurveType V> class BezierCurve {
public:
  static constexpr BezierCurveType valueType = V;

public:
  BezierCurve() { PC_PRINT("CREATED", TagType::Constr, "BezierCurve.h") };
  ~BezierCurve() { PC_PRINT("DESTROYED", TagType::Destr, "BezierCurve.h") };

private:
  float t = 0.0f; // The non-independent parameter value. [0-1.0f]
  // float delta = 0.0f;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
