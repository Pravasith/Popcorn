#pragma once

#include "CurveDefs.h"
#include "Curves.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <map>
#include <variant>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

using CurveHashType = uint32_t;

template <CurveValueType T> class CurveBank {
public:
  CurveBank() = default;

  // CreateCurve if it doesn't exist
  const LinearCurve<T> &GetCurve(const CurveInfoLinearForm<T> &curveInfo);
  const BezierCurve<T> &GetCurve(const CurveInfoBezierForm<T> &curveInfo);
  const HermiteCurve<T> &GetCurve(const CurveInfoHermiteForm<T> &curveInfo);

  const Curve<T> *GetCurvePtr(CurveHashType);

private:
  std::map<CurveHashType, LinearCurve<T>> m_linearCurves;
  std::map<CurveHashType, BezierCurve<T>> m_bezierCurves;
  std::map<CurveHashType, HermiteCurve<T>> m_hermiteCurves;
};

class CurveFactory {
public:
  [[nodiscard]] inline static CurveFactory *Get() {
    if (s_instance) {
      return s_instance;
    };

    s_instance = new CurveFactory();
    return s_instance;
  };

  static void Destroy() {
    if (s_instance) {
      delete s_instance;
      s_instance = nullptr;
    } else {
      PC_WARN("Trying to destroy a non-existant instance of CurveFactory")
    };
  };

public:
  // template <CurveValueType P>
  // [[nodiscard]] const Curve<P> *
  // MakeSpline(const std::string &name, const CurveInfoLinearForm<P>
  // &curveInfo) {
  // }

  template <CurveValueType P>
  [[nodiscard]] const Curve<P> *
  MakeCurve(const std::string &name, const CurveInfoLinearForm<P> &curveInfo) {
    if (m_curveLibrary.contains(name)) {
      throw std::runtime_error("Curve already exists: " + name);
    }

    LinearCurve<P> curve(curveInfo);
    m_curveLibrary[name] = curve;

    return GetCurve<P>(name);
  }

  template <CurveValueType P>
  [[nodiscard]] const Curve<P> *
  MakeCurve(const std::string &name, const CurveInfoBezierForm<P> &curveInfo) {
    if (m_curveLibrary.contains(name)) {
      throw std::runtime_error("Curve already exists: " + name);
    }

    BezierCurve<P> curve(curveInfo);
    m_curveLibrary[name] = curve;

    return GetCurve<P>(name);
  }

  template <CurveValueType P>
  [[nodiscard]] const Curve<P> *
  MakeCurve(const std::string &name, const CurveInfoHermiteForm<P> &curveInfo) {
    if (m_curveLibrary.contains(name)) {
      throw std::runtime_error("Curve already exists: " + name);
    }

    HermiteCurve<P> curve(curveInfo);
    m_curveLibrary[name] = curve;

    return GetCurve<P>(name);
  }

  template <CurveValueType P>
  const Curve<P> *GetCurve(const std::string &name) const {
    auto it = m_curveLibrary.find(name);
    if (it == m_curveLibrary.end())
      throw std::out_of_range("No such curve: " + name);

    // visit the variant and return desired type (in this case const Curve<P>*
    return std::visit(
        [](const auto &concreteCurveObj) -> const Curve<P> * {
          return &concreteCurveObj;
        },
        it->second // it->second is an std::variant (more specifically, the
                   // variant CurveType defined above)
    );
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
  using CurveType = std::variant<
      // float
      LinearCurve<float>, BezierCurve<float>, HermiteCurve<float>,
      // double
      LinearCurve<double>, BezierCurve<double>, HermiteCurve<double>,
      // vec2
      LinearCurve<glm::vec2>, BezierCurve<glm::vec2>, HermiteCurve<glm::vec2>,
      // vec3
      LinearCurve<glm::vec3>, BezierCurve<glm::vec3>, HermiteCurve<glm::vec3>,
      // vec4
      LinearCurve<glm::vec4>, BezierCurve<glm::vec4>, HermiteCurve<glm::vec4>>;

private:
  static CurveFactory *s_instance;

  // Curve<float> *curveFloat;
  // Curve<glm::vec2> *curveVec2;
  // Curve<glm::vec3> *curveVec3;
  // Curve<glm::vec4> *curveVec4;

  // Using map instead of unordered map bc map is more stable. When the buckets
  // qty increases, it reallocates leading to dangling ptrs (if object is later
  // accessed by ptr)
  std::map<std::string, CurveType> m_curveLibrary;

  CurveBank<float> m_floatCurves;
  CurveBank<double> m_doubleCurves;
  CurveBank<glm::vec2> m_vec2Curves;
  CurveBank<glm::vec3> m_vec3Curves;
  CurveBank<glm::vec4> m_vec4Curves;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
