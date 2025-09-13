#include "Transforms.h"
#include "GlobalMacros.h"
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// --- TRANSLATION --------------------------------------------------------
// --- TRANSLATION --------------------------------------------------------
//
void Transformations::TranslateLocal(const glm::vec3 &targetPos) {
  m_position.Set(targetPos, [&]() { UpdatePositionMatrix(); });
}

template <>
void Transformations::TranslateLocal<Axes::X>(float signedDistance) {
  m_position.AddComponent<Axes::X>(signedDistance,
                                   [&]() { UpdatePositionMatrix(); });
}
template <>
void Transformations::TranslateLocal<Axes::Y>(float signedDistance) {
  m_position.AddComponent<Axes::Y>(signedDistance,
                                   [&]() { UpdatePositionMatrix(); });
}
template <>
void Transformations::TranslateLocal<Axes::Z>(float signedDistance) {
  m_position.AddComponent<Axes::Z>(signedDistance,
                                   [&]() { UpdatePositionMatrix(); });
}

//
// --- ROTATION -----------------------------------------------------------
// --- ROTATION -----------------------------------------------------------
//
void Transformations::RotateLocalEuler(const glm::vec3 &rotationEuler) {
  glm::quat dq = glm::quat(rotationEuler);
  m_rotationQuat.Set(dq * m_rotationQuat.GetValue());
  UpdateRotationMatrix();
};

template <> void Transformations::RotateLocalEuler<Axes::X>(float radians) {
  glm::quat dq = glm::angleAxis(radians, glm::vec3{1.0, 0.0, 0.0});
  m_rotationQuat.Set(dq * m_rotationQuat.GetValue());
}
template <> void Transformations::RotateLocalEuler<Axes::Y>(float radians) {
  glm::quat dq = glm::angleAxis(radians, glm::vec3{0.0, 1.0, 0.0});
  m_rotationQuat.Set(dq * m_rotationQuat.GetValue());
}
template <> void Transformations::RotateLocalEuler<Axes::Z>(float radians) {
  glm::quat dq = glm::angleAxis(radians, glm::vec3{0.0, 0.0, 1.0});
  m_rotationQuat.Set(dq * m_rotationQuat.GetValue());
}

//
// --- SCALE --------------------------------------------------------------
// --- SCALE --------------------------------------------------------------
//
void Transformations::ScaleLocal(float scalarValue) {
  m_scale.MultiplyComponent<Axes::X>(scalarValue);
  m_scale.MultiplyComponent<Axes::Y>(scalarValue);
  m_scale.MultiplyComponent<Axes::Z>(scalarValue,
                                     [&]() { UpdateScaleMatrix(); });
}

void Transformations::ScaleLocal(const glm::vec3 &scaleVector) {
  m_scale.Set(scaleVector, [&]() { UpdateScaleMatrix(); });
}

template <> void Transformations::ScaleLocal<Axes::X>(float scalarValue) {
  m_scale.MultiplyComponent<Axes::X>(scalarValue,
                                     [&]() { UpdateScaleMatrix(); });
}
template <> void Transformations::ScaleLocal<Axes::Y>(float scalarValue) {
  m_scale.MultiplyComponent<Axes::Y>(scalarValue,
                                     [&]() { UpdateScaleMatrix(); });
}
template <> void Transformations::ScaleLocal<Axes::Z>(float scalarValue) {
  m_scale.MultiplyComponent<Axes::Z>(scalarValue,
                                     [&]() { UpdateScaleMatrix(); });
}

//
// --- MATRIX STUFF -------------------------------------------------------
// --- MATRIX STUFF -------------------------------------------------------
//
void Transformations::UpdatePositionMatrix() {
  m_translationMatrix = glm::translate(PC_IDENTITY_MAT4, m_position.GetValue());
  UpdateLocalMatrix();
}

void Transformations::UpdateRotationMatrix() {
  m_rotationMatrix = glm::mat4_cast(m_rotationQuat.GetValue());

  UpdateLookAtDirection();
  UpdateLocalMatrix();
}

void Transformations::UpdateScaleMatrix() {
  m_scaleMatrix = glm::scale(PC_IDENTITY_MAT4, m_scale.GetValue());
  UpdateLookAtDirection();
  UpdateLocalMatrix();
};

void Transformations::UpdateLocalMatrix() {
  m_localMatrix = m_translationMatrix * m_rotationMatrix * m_scaleMatrix;
  m_worldMatrixNeedsUpdate = true;
}

void Transformations::SetLocalMatrix(const glm::mat4 &mat) {
  m_localMatrix = mat;
  m_worldMatrixNeedsUpdate = true;
};

// update world matrix wrt parent
void Transformations::UpdateWorldMatrix(const glm::mat4 &parentWorldMatrix) {
  m_worldMatrix = parentWorldMatrix * m_localMatrix;
  m_worldMatrixNeedsUpdate = false;
}

void Transformations::UpdateLookAtDirection() {
  // glm::mat4 rotMat = m_rotationMatrix;
  glm::vec3 initialLookAt{0.f, 0.f, -1.f}; // facing the world -Z

  m_lookAtDir =
      // glm::normalize(glm::vec3(rotMat * glm::vec4(initialLookAt, 0.f)));
      glm::normalize(glm::mat3(m_rotationMatrix) * initialLookAt);
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
