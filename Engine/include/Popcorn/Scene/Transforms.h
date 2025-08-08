#pragma once

#include "GlobalMacros.h"
#include "MathConstants.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

enum class Axes { X = 1, Y, Z };

// enum class Transforms { Translate = 1, Rotate, Scale, Shear, Reflect };

enum class EulerOrder {
  XYZ = 1,
  XZY,
  YXZ,
  YZX,
  ZXY,
  ZYX,
};

class Transformations {
public:
  // --- positioning ----------------------------------------------------------
  void TranslateLocal(const glm::vec3 &targetPos);
  template <Axes T> void TranslateLocal(float signedDistance);
  void UpdatePositionMatrix();

  // --- rotations ------------------------------------------------------------
  void SetEulerOrder(EulerOrder order) { m_eulerOrder = order; };
  void RotateLocalEuler(const glm::vec3 &rotationEuler);
  template <Axes T> void RotateLocalEuler(float radians);
  void UpdateRotationMatrix();

  // --- scaling --------------------------------------------------------------
  template <Axes T> void ScaleLocal(float scalarValue);
  void ScaleLocal(float scalarValue);
  void ScaleLocal(const glm::vec3 &scaleVector);
  void UpdateScaleMatrix();

  void UpdateLocalMatrix();
  void SetLocalMatrix(const glm::mat4 &mat);

  // update world matrix wrt parent
  void UpdateWorldMatrix(const glm::mat4 &parentWorldMatrix);
  void SetWorldMatrixNeedsUpdate(bool val) { m_worldMatrixNeedsUpdate = val; }
  void UpdateLookAtDirection();

public:
  Transformations() = default;
  ~Transformations() = default;

public:
  Transformations(const Transformations &) = delete;
  Transformations &operator=(const Transformations &) = delete;

  Transformations(Transformations &&other) noexcept {
    m_eulerOrder = other.m_eulerOrder;

    m_position = other.m_position;
    m_rotationEuler = other.m_rotationEuler;
    m_scale = other.m_scale;

    m_translationMatrix = other.m_translationMatrix;
    m_rotationMatrix = other.m_rotationMatrix;
    m_scaleMatrix = other.m_scaleMatrix;

    m_localMatrix = other.m_localMatrix;
    m_worldMatrix = other.m_worldMatrix;

    m_lookAtDir = other.m_lookAtDir;

    m_worldMatrixNeedsUpdate = other.m_worldMatrixNeedsUpdate;
  };

  Transformations &operator=(Transformations &&other) noexcept {
    if (this != &other) {
      m_eulerOrder = other.m_eulerOrder;

      m_position = other.m_position;
      m_rotationEuler = other.m_rotationEuler;
      m_scale = other.m_scale;

      m_translationMatrix = other.m_translationMatrix;
      m_rotationMatrix = other.m_rotationMatrix;
      m_scaleMatrix = other.m_scaleMatrix;

      m_localMatrix = other.m_localMatrix;
      m_worldMatrix = other.m_worldMatrix;

      m_lookAtDir = other.m_lookAtDir;

      m_worldMatrixNeedsUpdate = other.m_worldMatrixNeedsUpdate;
    };
    return *this;
  };

public:
  EulerOrder m_eulerOrder = EulerOrder::XYZ;

  glm::vec3 m_position{0, 0, 0};
  glm::vec3 m_rotationEuler{0, 0, 0};
  glm::vec3 m_scale{1, 1, 1};

  glm::mat4 m_translationMatrix = PC_IDENTITY_MAT4;
  glm::mat4 m_rotationMatrix = PC_IDENTITY_MAT4;
  glm::mat4 m_scaleMatrix = PC_IDENTITY_MAT4;

  glm::mat4 m_localMatrix = PC_IDENTITY_MAT4; // Local -> Parent
  glm::mat4 m_worldMatrix = PC_IDENTITY_MAT4; // Local -> World

  glm::vec3 m_lookAtDir{0.f, 0.f, -1.f}; // towards the screen

  bool m_worldMatrixNeedsUpdate = false;
};

//
template <> void Transformations::TranslateLocal<Axes::X>(float);
template <> void Transformations::TranslateLocal<Axes::Y>(float);
template <> void Transformations::TranslateLocal<Axes::Z>(float);

template <> void Transformations::RotateLocalEuler<Axes::X>(float);
template <> void Transformations::RotateLocalEuler<Axes::Y>(float);
template <> void Transformations::RotateLocalEuler<Axes::Z>(float);

template <> void Transformations::ScaleLocal<Axes::X>(float);
template <> void Transformations::ScaleLocal<Axes::Y>(float);
template <> void Transformations::ScaleLocal<Axes::Z>(float);

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
