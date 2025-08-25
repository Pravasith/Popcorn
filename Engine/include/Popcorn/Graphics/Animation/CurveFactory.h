#pragma once

#include "CurveDefs.h"
#include "CurveUtils.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <glm/ext/vector_float2.hpp>
#include <map>
#include <type_traits>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
//
//
//
// -------------------------------------------------------------------
// --- CURVE BANK ----------------------------------------------------
//
template <CurveValueType T> class CurveBank {
public:
  CurveBank() = default;
  ~CurveBank() = default;

#define GET_CREATE_CURVE_PTR(CURVE_CONTAINER)                                  \
  do {                                                                         \
    const CurveHashType curveHash = HashCurveInfo(curveInfo);                  \
    auto [it, _isInserted] =                                                   \
        (CURVE_CONTAINER).try_emplace(curveHash, curveInfo);                   \
    return &it->second;                                                        \
  } while (0); // (it) is an iterator of std::pair<Key, Val>s
  // Creates curve if it doesn't exist
  const LinearCurve<T> *GetCurvePtr(const CurveInfoLinearForm<T> &curveInfo) {
    GET_CREATE_CURVE_PTR(m_linearCurves)
  }
  // Creates curve if it doesn't exist
  const BezierCurve<T> *GetCurvePtr(const CurveInfoBezierForm<T> &curveInfo) {
    GET_CREATE_CURVE_PTR(m_bezierCurves)
  }
  // Creates curve if it doesn't exist
  const HermiteCurve<T> *GetCurvePtr(const CurveInfoHermiteForm<T> &curveInfo) {
    GET_CREATE_CURVE_PTR(m_hermiteCurves)
  }
#undef GET_CREATE_CURVE_PTR

  static inline CurveHashType
  HashCurveInfo(const CurveInfoLinearForm<T> &curveInfo) {
    return PC_HashCurveInfo_Linear(curveInfo);
  }
  static inline CurveHashType
  HashCurveInfo(const CurveInfoBezierForm<T> &curveInfo) {
    return PC_HashCurveInfo_Bezier(curveInfo);
  }
  static inline CurveHashType
  HashCurveInfo(const CurveInfoHermiteForm<T> &curveInfo) {
    return PC_HashCurveInfo_Hermite(curveInfo);
  }

  void CleanUp() {
    m_linearCurves.clear();
    m_bezierCurves.clear();
    m_hermiteCurves.clear();
  }

private:
  // Not using unordered_map bc we're accessing curves with ptrs
  std::map<CurveHashType, LinearCurve<T>> m_linearCurves;
  std::map<CurveHashType, BezierCurve<T>> m_bezierCurves;
  std::map<CurveHashType, HermiteCurve<T>> m_hermiteCurves;
};

//
//
//
//
// -------------------------------------------------------------------
// --- CURVE FACTORY -------------------------------------------------
//
class CurveFactory {
public:
#define GET_PTR_FROM_CURVE_BANK                                                \
  if constexpr (std::is_same_v<T, float>) {                                    \
    return m_floatCurves.GetCurvePtr(curveInfo);                               \
  } else if constexpr (std::is_same_v<T, double>) {                            \
    return m_doubleCurves.GetCurvePtr(curveInfo);                              \
  } else if constexpr (std::is_same_v<T, glm::vec2>) {                         \
    return m_vec2Curves.GetCurvePtr(curveInfo);                                \
  } else if constexpr (std::is_same_v<T, glm::vec3>) {                         \
    return m_vec3Curves.GetCurvePtr(curveInfo);                                \
  } else if constexpr (std::is_same_v<T, glm::vec4>) {                         \
    return m_vec4Curves.GetCurvePtr(curveInfo);                                \
  }

  template <CurveValueType T>
  // Creates curve if it doesn't exist
  const LinearCurve<T> *GetCurvePtr(const CurveInfoLinearForm<T> &curveInfo) {
    GET_PTR_FROM_CURVE_BANK
  }
  template <CurveValueType T>
  // Creates curve if it doesn't exist
  const BezierCurve<T> *GetCurvePtr(const CurveInfoBezierForm<T> &curveInfo) {
    GET_PTR_FROM_CURVE_BANK
  }
  template <CurveValueType T>
  // Creates curve if it doesn't exist
  const HermiteCurve<T> *GetCurvePtr(const CurveInfoHermiteForm<T> &curveInfo) {
    GET_PTR_FROM_CURVE_BANK
  }

public:
  [[nodiscard]] inline static CurveFactory *Get() {
    if (s_instance) {
      return s_instance;
    }

    s_instance = new CurveFactory();
    return s_instance;
  }

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of CurveFactory")
    }
  }

private:
  CurveFactory() { PC_PRINT("CREATED", TagType::Constr, "CurveFactory") }
  ~CurveFactory() {
    m_floatCurves.CleanUp();
    m_doubleCurves.CleanUp();
    m_vec2Curves.CleanUp();
    m_vec3Curves.CleanUp();
    m_vec4Curves.CleanUp();
    PC_PRINT("DESTROYED", TagType::Destr, "CurveFactory")
  }

  CurveFactory(const CurveFactory &) = delete;
  CurveFactory &operator=(const CurveFactory &) = delete;
  CurveFactory(CurveFactory &&) = delete;
  CurveFactory &operator=(CurveFactory &&) = delete;

private:
  static CurveFactory *s_instance;

  CurveBank<float> m_floatCurves;
  CurveBank<double> m_doubleCurves;
  CurveBank<glm::vec2> m_vec2Curves;
  CurveBank<glm::vec3> m_vec3Curves;
  CurveBank<glm::vec4> m_vec4Curves;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
