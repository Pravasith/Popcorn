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

enum class Transforms { Translate = 1, Rotate, Scale, Shear, Reflect };

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
  void SetPosition(glm::vec3 pos) {
    m_position = pos;
    UpdatePositionMatrix();
  }

  void Translate(float signedDistance, Axes axis) {
    switch (axis) {
    case Axes::X:
      m_position.x += signedDistance;
      break;
    case Axes::Y:
      m_position.y += signedDistance;
      break;
    case Axes::Z:
      m_position.z += signedDistance;
      break;
    }

    UpdatePositionMatrix();
  };

  void UpdatePositionMatrix() {
    m_translationMatrix = glm::translate(PC_IDENTITY_MAT4, m_position);
    UpdateLocalMatrix();
  }

  void SetEulerOrder(EulerOrder order) { m_eulerOrder = order; };

  template <Axes T> void RotateEuler(float radians) {
    if constexpr (T == Axes::X) {
      m_rotationEuler.x += radians;
    } else if constexpr (T == Axes::Y) {
      m_rotationEuler.y += radians;
    } else if constexpr (T == Axes::Z) {
      m_rotationEuler.z += radians;
    }
    UpdateRotationMatrix();
  }

  void SetRotationEuler(glm::vec3 rotationEuler) {
    m_rotationEuler = rotationEuler;
    UpdateRotationMatrix();
  };

  void UpdateLocalMatrix() {
    m_localMatrix = m_translationMatrix * m_rotationMatrix * m_scaleMatrix;
    m_worldMatrixNeedsUpdate = true;
  };

  void UpdateRotationMatrix() {
    auto rotX = glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.x, {1, 0, 0});
    auto rotY = glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.y, {0, 1, 0});
    auto rotZ = glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.z, {0, 0, 1});

    switch (m_eulerOrder) {
    case EulerOrder::XYZ:
      m_rotationMatrix = rotX * rotY * rotZ;
      break;
    case EulerOrder::XZY:
      m_rotationMatrix = rotX * rotZ * rotY;
      break;
    case EulerOrder::YXZ:
      m_rotationMatrix = rotY * rotX * rotZ;
      break;
    case EulerOrder::YZX:
      m_rotationMatrix = rotY * rotZ * rotX;
      break;
    case EulerOrder::ZXY:
      m_rotationMatrix = rotZ * rotX * rotY;
      break;
    case EulerOrder::ZYX:
      m_rotationMatrix = rotZ * rotY * rotX;
      break;
    default: // XYZ
      m_rotationMatrix = rotX * rotY * rotZ;
      break;
    }

    UpdateLookAtDirection();
    UpdateLocalMatrix();
  }

  template <Axes T> void ScaleAlongAxis(float scalarValue) {
    if constexpr (T == Axes::X) {
      m_scale.x *= scalarValue;
    } else if constexpr (T == Axes::Y) {
      m_scale.y *= scalarValue;
    } else if constexpr (T == Axes::Z) {
      m_scale.z *= scalarValue;
    }
    UpdateScaleMatrix();
  };

  void ScaleUniformly(float scalarValue) {
    m_scale.x *= scalarValue;
    m_scale.y *= scalarValue;
    m_scale.z *= scalarValue;
    UpdateScaleMatrix();
  };

  void ScaleByValue(glm::vec3 scaleVector) {
    m_scale = scaleVector;
    UpdateScaleMatrix();
  };

  void SetLocalMatrix(const glm::mat4 &mat) {
    m_localMatrix = mat;
    m_worldMatrixNeedsUpdate = true;
  };

  // update world matrix wrt parent
  void UpdateWorldMatrix(const glm::mat4 &parentWorldMatrix) {
    m_worldMatrix = parentWorldMatrix * m_localMatrix;
    m_worldMatrixNeedsUpdate = false;
  };

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

  void SetWorldMatrixNeedsUpdate(bool val) { m_worldMatrixNeedsUpdate = val; }

  void UpdateScaleMatrix() {
    m_scaleMatrix = glm::scale(PC_IDENTITY_MAT4, m_scale);
    UpdateLookAtDirection();
    UpdateLocalMatrix();
  };

  void UpdateLookAtDirection() {
    glm::mat4 rotScale = m_rotationMatrix * m_scaleMatrix;
    glm::vec3 initialLookAt{0.f, 0.f, -1.f}; // facing the world -Z

    m_lookAtDir =
        glm::normalize(glm::vec3(rotScale * glm::vec4(initialLookAt, 0.f)));
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

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
