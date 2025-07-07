#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum CurveType {
  Hermite,
  Bezier,
};

enum SplineType {
  CatmullRom,
  BSpline,
};

struct AnimationProperty {};

//
//
// -------------------------------------------------------------
// --- CURVE ---------------------------------------------------
// -------------------------------------------------------------

class Curve {
public:
  Curve(float degree = 3.0f)
      : m_degree(degree) {
          PC_PRINT("CREATED", TagType::Constr, "Curve")
        };
  ~Curve() { PC_PRINT("DESTROYED", TagType::Destr, "Curve") };

  void FollowPath(GameObject &gameObject);
  void FollowPath(AnimationProperty &animationProp);

  // -----------------------------------------------------------
  // --- BEZIER ------------------------------------------------

  // -----------------------------------------------------------
  // --- HERMITE -----------------------------------------------

protected:
  float m_degree = 1.0f; // linear
  float m_t = 0.0f;      // 'the' parameter. t as in p(t); t E [0, 1]
};

//
//
//
// -------------------------------------------------------------
// --- SPLINE --------------------------------------------------
// -------------------------------------------------------------

template <SplineType V> class Spline {
public:
  static constexpr SplineType typeValue = V;

public:
  Spline() { PC_PRINT("CREATED", TagType::Constr, "Spline") };
  ~Spline() { PC_PRINT("DESTROYED", TagType::Destr, "Spline") };

  void FollowPath(GameObject &gameObject);
  void FollowPath(AnimationProperty &animationProp);

  // -----------------------------------------------------------
  // --- BEZIER ------------------------------------------------

  // -----------------------------------------------------------
  // --- HERMITE -----------------------------------------------

protected:
  float m_globalT = 0.0f; // 'the' parameter. t as in p(t)
  std::vector<Curve *> m_knots{};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
