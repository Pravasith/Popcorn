#pragma once

#include "Animation.h"
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

// clang-format off
// enum class Transforms { Translate = 1, Rotate, Scale, Shear, Reflect };
enum class Axes { X = 0, Y = 1, Z = 2, W = 3 };
enum class EulerOrder { XYZ = 1, XZY, YXZ, YZX, ZXY, ZYX };
// clang-format on

class Transformations {
public:
  // --- animations -----------------------------------------------------------
  void LinkAnimatableProperty();

  // --- positioning ----------------------------------------------------------
  void TranslateLocal(const glm::vec3 &targetPos);
  template <Axes T> void TranslateLocal(float signedDistance);

  // --- rotations ------------------------------------------------------------
  void SetEulerOrder(EulerOrder order) { m_eulerOrder = order; };
  void RotateLocalEuler(const glm::vec3 &rotationEuler);
  template <Axes T> void RotateLocalEuler(float radians);

  // --- scaling --------------------------------------------------------------
  template <Axes T> void ScaleLocal(float scalarValue);
  void ScaleLocal(float scalarValue);
  void ScaleLocal(const glm::vec3 &scaleVector);

  // --- matrix stuff ---------------------------------------------------------
  void SetLocalMatrix(const glm::mat4 &mat); // only for GltfLoader
  void SetWorldMatrixNeedsUpdate(bool val) { m_worldMatrixNeedsUpdate = val; }
  //
  void UpdatePositionMatrix();
  void UpdateRotationMatrix();
  void UpdateScaleMatrix();
  void UpdateLocalMatrix();
  void UpdateWorldMatrix(const glm::mat4 &parentWorldMatrix);

  // --- lookAt stuff ---------------------------------------------------------
  void UpdateLookAtDirection();

public:
  Transformations() = default;
  ~Transformations() = default;

  Transformations(const Transformations &) = delete;
  Transformations &operator=(const Transformations &) = delete;
  Transformations(Transformations &&other) = default;
  Transformations &operator=(Transformations &&other) = default;

public:
  EulerOrder m_eulerOrder = EulerOrder::XYZ;

  AnimationProperty<glm::vec3> m_position2{0, 0, 0};

  // glm::vec3 m_position{0, 0, 0};
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
