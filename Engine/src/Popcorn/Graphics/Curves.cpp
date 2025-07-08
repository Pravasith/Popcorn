#include "Curves.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
// ------------------------------------------------------------------------
// --- LINEAR -------------------------------------------------------------

//
//
// ------------------------------------------------------------------------
// --- BEZIER -------------------------------------------------------------
// template <>
// double Curve::GetValueAt_Slow<CurveType::Bezier>(float t) {
//   // De Casteljau's algorithm
//
// };

// template <>
// float Curve::GetValueAt_Fast<CurveType::Bezier>(float t) {
//   // Bernstein basis
// };

//
//
// ------------------------------------------------------------------------
// --- HERMITE ------------------------------------------------------------
// template <> double Curve::GetValueAt_Slow<CurveType::Hermite>(float t) {};
// template <> float Curve::GetValueAt_Fast<CurveType::Hermite>(float t) {};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
