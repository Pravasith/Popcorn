#pragma once

#include "CurveDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <glm/ext/vector_float2.hpp>
#include <map>
#include <type_traits>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

template <CurveValueType T> class CurveBank {
public:
  CurveBank() = default;

#define GET_INSERT_CURVE_PTR(CURVE_CONTAINER)                                  \
  const CurveHashType curveHash = HashCurveInfo(curveInfo);                    \
  auto [it, _isInserted] =                                                     \
      (CURVE_CONTAINER).try_emplace(curveHash, curveInfo);                     \
  return &it->second; // it is an iterator of std::pair<Key, Val>s

  // Creates curve if it doesn't exist
  const LinearCurve<T> *GetCurvePtr(const CurveInfoLinearForm<T> &curveInfo) {
    GET_INSERT_CURVE_PTR(m_linearCurves)
  }
  // Creates curve if it doesn't exist
  const BezierCurve<T> *GetCurvePtr(const CurveInfoBezierForm<T> &curveInfo) {
    GET_INSERT_CURVE_PTR(m_bezierCurves)
  }
  // Creates curve if it doesn't exist
  const HermiteCurve<T> *GetCurvePtr(const CurveInfoHermiteForm<T> &curveInfo){
      GET_INSERT_CURVE_PTR(m_hermiteCurves)}

#define HASH_CURVE_BODY                                                        \
  do {                                                                         \
    CurveHashType hash = 0;                                                    \
    if constexpr (std::is_same_v<T, float>) {                                  \
      auto p0 = PC_FloatToUint64(curveInfo.p0);                                \
      auto p1 = PC_FloatToUint64(curveInfo.p1);                                \
      hash ^= p0;                                                              \
      hash ^= p1 >> 3;                                                         \
    } else if constexpr (std::is_same_v<T, double>) {                          \
      auto p0 = PC_DoubleToUint64(curveInfo.p0);                               \
      auto p1 = PC_DoubleToUint64(curveInfo.p1);                               \
      hash ^= p0;                                                              \
      hash ^= p1 >> 3;                                                         \
    } else if constexpr (std::is_same_v<T, glm::vec2>) {                       \
      auto p0X = PC_FloatToUint64(curveInfo.p0.x);                             \
      auto p0Y = PC_FloatToUint64(curveInfo.p0.y);                             \
      auto p1X = PC_FloatToUint64(curveInfo.p1.x);                             \
      auto p1Y = PC_FloatToUint64(curveInfo.p1.y);                             \
      hash ^= p0X << 3;                                                        \
      hash ^= p0Y << 2;                                                        \
      hash ^= p1X;                                                             \
      hash ^= p1Y >> 3;                                                        \
    } else if constexpr (std::is_same_v<T, glm::vec3>) {                       \
      auto p0X = PC_FloatToUint64(curveInfo.p0.x);                             \
      auto p0Y = PC_FloatToUint64(curveInfo.p0.y);                             \
      auto p0Z = PC_FloatToUint64(curveInfo.p0.z);                             \
      auto p1X = PC_FloatToUint64(curveInfo.p1.x);                             \
      auto p1Y = PC_FloatToUint64(curveInfo.p1.y);                             \
      auto p1Z = PC_FloatToUint64(curveInfo.p1.z);                             \
      hash ^= p0X << 3;                                                        \
      hash ^= p0Y << 2;                                                        \
      hash ^= p0Z;                                                             \
      hash ^= p1X >> 2;                                                        \
      hash ^= p1Y >> 3;                                                        \
      hash ^= p1Z >> 5;                                                        \
    } else if constexpr (std::is_same_v<T, glm::vec4>) {                       \
      auto p0X = PC_FloatToUint64(curveInfo.p0.x);                             \
      auto p0Y = PC_FloatToUint64(curveInfo.p0.y);                             \
      auto p0Z = PC_FloatToUint64(curveInfo.p0.z);                             \
      auto p0W = PC_FloatToUint64(curveInfo.p0.w);                             \
      auto p1X = PC_FloatToUint64(curveInfo.p1.x);                             \
      auto p1Y = PC_FloatToUint64(curveInfo.p1.y);                             \
      auto p1Z = PC_FloatToUint64(curveInfo.p1.z);                             \
      auto p1W = PC_FloatToUint64(curveInfo.p1.w);                             \
      hash ^= p0X << 4;                                                        \
      hash ^= p0Y << 3;                                                        \
      hash ^= p0Z << 2;                                                        \
      hash ^= p0W;                                                             \
      hash ^= p1X >> 2;                                                        \
      hash ^= p1Y >> 3;                                                        \
      hash ^= p1Z >> 5;                                                        \
      hash ^= p1W >> 7;                                                        \
    }                                                                          \
    return hash * 2654435761ull;                                               \
  } while (0);

  static CurveHashType HashCurveInfo(const CurveInfoLinearForm<T> &curveInfo) {
    HASH_CURVE_BODY
  }
  static CurveHashType HashCurveInfo(const CurveInfoBezierForm<T> &curveInfo) {
    HASH_CURVE_BODY
  }
  static CurveHashType HashCurveInfo(const CurveInfoHermiteForm<T> &curveInfo) {
    HASH_CURVE_BODY
  }

private:
  // Not using unordered_map bc we're accessing curves with ptrs
  std::map<CurveHashType, LinearCurve<T>> m_linearCurves;
  std::map<CurveHashType, BezierCurve<T>> m_bezierCurves;
  std::map<CurveHashType, HermiteCurve<T>> m_hermiteCurves;
};

class CurveFactory {
public:
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
  const LinearCurve<T> *GetCurvePtr(const CurveInfoLinearForm<T> &curveInfo) {
    GET_PTR_FROM_CURVE_BANK
  }
  template <CurveValueType T>
  const BezierCurve<T> *GetCurvePtr(const CurveInfoBezierForm<T> &curveInfo) {
    GET_PTR_FROM_CURVE_BANK
  }
  template <CurveValueType T>
  const HermiteCurve<T> *GetCurvePtr(const CurveInfoHermiteForm<T> &curveInfo) {
    GET_PTR_FROM_CURVE_BANK
  }

private:
  CurveFactory() { PC_PRINT("CREATED", TagType::Constr, "CurveFactory") }
  ~CurveFactory(){// TODO: Delete curves in the library (static alloc)
                  PC_PRINT("DESTROYED", TagType::Destr, "CurveFactory")}

  // DELETE THE COPY CONSTRUCTOR AND COPY ASSIGNMENT OPERATOR
  CurveFactory(const CurveFactory &) = delete;
  CurveFactory &operator=(const CurveFactory &) = delete;

  // DELETE THE MOVE CONSTRUCTOR AND MOVE ASSIGNMENT OPERATOR
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
