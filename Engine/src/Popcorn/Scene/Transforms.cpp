#include "Transforms.h"
#include "GlobalMacros.h"
#include "SceneDefs.h"
#include <cassert>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// --- TRANSLATION --------------------------------------------------------
// --- TRANSLATION --------------------------------------------------------
//
void Transformations::TranslateLocal(const glm::vec3 &targetPos) {
  m_position.Set(targetPos);

  // UpdatePositionDependents();
}

template <>
void Transformations::TranslateLocal<Axes::X>(float signedDistance) {
  m_position.AddComponent<Axes::X>(signedDistance);

  // UpdatePositionDependents();
}
template <>
void Transformations::TranslateLocal<Axes::Y>(float signedDistance) {
  m_position.AddComponent<Axes::Y>(signedDistance);

  // UpdatePositionDependents();
}
template <>
void Transformations::TranslateLocal<Axes::Z>(float signedDistance) {
  m_position.AddComponent<Axes::Z>(signedDistance);

  // UpdatePositionDependents();
}

//
// --- ROTATION -----------------------------------------------------------
// --- ROTATION -----------------------------------------------------------
//
void Transformations::RotateLocalEuler(const glm::vec3 &rotationEuler) {
  glm::quat dq = glm::quat(rotationEuler);
  m_rotationQuat.Set(dq * m_rotationQuat.GetValue());

  // UpdateRotationDependents();
}

template <> void Transformations::RotateLocalEuler<Axes::X>(float radians) {
  glm::quat dq = glm::angleAxis(radians, glm::vec3{1.0, 0.0, 0.0});
  m_rotationQuat.Set(dq * m_rotationQuat.GetValue());

  // UpdateRotationDependents();
}
template <> void Transformations::RotateLocalEuler<Axes::Y>(float radians) {
  glm::quat dq = glm::angleAxis(radians, glm::vec3{0.0, 1.0, 0.0});
  m_rotationQuat.Set(dq * m_rotationQuat.GetValue());

  // UpdateRotationDependents();
}
template <> void Transformations::RotateLocalEuler<Axes::Z>(float radians) {
  glm::quat dq = glm::angleAxis(radians, glm::vec3{0.0, 0.0, 1.0});
  m_rotationQuat.Set(dq * m_rotationQuat.GetValue());

  // UpdateRotationDependents();
}

void Transformations::RotateLocalQuat(const glm::quat &dq) {
  m_rotationQuat.Set(glm::normalize(dq * m_rotationQuat.GetValue()));

  // UpdateRotationDependents();
}
void Transformations::SetRotationQuat(const glm::quat &q) {
  m_rotationQuat.Set(glm::normalize(q));

  // UpdateRotationDependents();
}

//
// --- SCALE --------------------------------------------------------------
// --- SCALE --------------------------------------------------------------
//
void Transformations::ScaleLocal(float scalarValue) {
  m_scale.MultiplyComponent<Axes::X>(scalarValue);
  m_scale.MultiplyComponent<Axes::Y>(scalarValue);
  m_scale.MultiplyComponent<Axes::Z>(scalarValue);

  // UpdateScaleDependents();
}

void Transformations::ScaleLocal(const glm::vec3 &scaleVector) {
  m_scale.Set(scaleVector);

  // UpdateScaleDependents();
}

template <> void Transformations::ScaleLocal<Axes::X>(float scalarValue) {
  m_scale.MultiplyComponent<Axes::X>(scalarValue);

  // UpdateScaleDependents();
}
template <> void Transformations::ScaleLocal<Axes::Y>(float scalarValue) {
  m_scale.MultiplyComponent<Axes::Y>(scalarValue);

  // UpdateScaleDependents();
}
template <> void Transformations::ScaleLocal<Axes::Z>(float scalarValue) {
  m_scale.MultiplyComponent<Axes::Z>(scalarValue);

  // UpdateScaleDependents();
}

//
// --- MATRIX STUFF -------------------------------------------------------
// --- MATRIX STUFF -------------------------------------------------------
//
void Transformations::UpdatePositionMatrix() {
  m_translationMatrix = glm::translate(PC_IDENTITY_MAT4, m_position.GetValue());
}

void Transformations::UpdateRotationMatrix() {
  m_rotationMatrix = glm::mat4_cast(m_rotationQuat.GetValue());
}

void Transformations::UpdateScaleMatrix() {
  m_scaleMatrix = glm::scale(PC_IDENTITY_MAT4, m_scale.GetValue());
}

void Transformations::UpdateLocalMatrix() {
  m_localMatrix = m_translationMatrix * m_rotationMatrix * m_scaleMatrix;

  // local matrix dependents
  m_worldMatrixNeedsUpdate = true;

  assert(m_gameObjChildWorldMatUpdateFlag_Cb);
  m_gameObjChildWorldMatUpdateFlag_Cb();
}

void Transformations::SetLocalMatrix(const glm::mat4 &mat) {
  m_localMatrix = mat;
  m_worldMatrixNeedsUpdate = true;

  assert(m_gameObjChildWorldMatUpdateFlag_Cb);
  m_gameObjChildWorldMatUpdateFlag_Cb();
}

// update world matrix wrt parent
void Transformations::UpdateWorldMatrix(const glm::mat4 &parentWorldMatrix) {
  m_worldMatrix = parentWorldMatrix * m_localMatrix;
  m_worldMatrixNeedsUpdate = false;
}

//
// --- LOOKAT STUFF -------------------------------------------------------
// --- LOOKAT STUFF -------------------------------------------------------
//
void Transformations::SetLookAtDirection(const glm::vec3 &lookAtDir) {
  // Indirectly updates m_lookAtDir again through nested methods (no harm):
  // Sets new quat --> sets new rot matrix --> sets new lookAtDir
  m_lookAtDir = lookAtDir;

  glm::quat newOrientation =
      glm::quatLookAtRH(glm::normalize(m_lookAtDir), PC_WORLD_UP_DIR);
  SetRotationQuat(
      glm::normalize(newOrientation)); // sets rot matrix which sets m_lookAtDir
                                       // again (no harm)
}

void Transformations::UpdateLookAtDirection() {
  // NOTE: DON'T CHANGE THIS
  // Extracts lookAtDir from the quat orientation
  glm::vec3 initialLookAt{0.f, 0.f, -1.f}; // facing the world -Z
  m_lookAtDir = glm::normalize(m_rotationQuat.GetValue() * initialLookAt);
}

//
// --- DEPENDENTS STUFF ---------------------------------------------------
// --- DEPENDENTS STUFF ---------------------------------------------------
//
// Fires as an AfterMorphCb for position animation property
void Transformations::UpdatePositionDependents() {
  UpdatePositionMatrix();
  UpdateLocalMatrix();

  // For camera, update view matrix
  // if (m_cameraViewMatrixUpdate_Cb) {
  //   m_cameraViewMatrixUpdate_Cb();
  // }
}
// Fires as an AfterMorphCb for rotation animation property
void Transformations::UpdateRotationDependents() {
  UpdateRotationMatrix();
  UpdateLookAtDirection();
  UpdateLocalMatrix();

  // For camera, update view matrix
  // if (m_cameraViewMatrixUpdate_Cb) {
  //   m_cameraViewMatrixUpdate_Cb();
  // }
}
// Fires as an AfterMorphCb for scale animation property
void Transformations::UpdateScaleDependents() {
  UpdateScaleMatrix();
  UpdateLocalMatrix();
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
