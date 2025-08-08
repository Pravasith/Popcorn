#include "Transforms.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

//
// --- TRANSLATION --------------------------------------------------------
// --- TRANSLATION --------------------------------------------------------
//
void Transformations::TranslateLocal(const glm::vec3 &targetPos) {
  m_position = targetPos;
  UpdatePositionMatrix();
}

template <>
void Transformations::TranslateLocal<Axes::X>(float signedDistance) {
  m_position.x += signedDistance;
  UpdatePositionMatrix();
}
template <>
void Transformations::TranslateLocal<Axes::Y>(float signedDistance) {
  m_position.y += signedDistance;
  UpdatePositionMatrix();
}
template <>
void Transformations::TranslateLocal<Axes::Z>(float signedDistance) {
  m_position.z += signedDistance;
  UpdatePositionMatrix();
}

//
// --- ROTATION -----------------------------------------------------------
// --- ROTATION -----------------------------------------------------------
//
void Transformations::RotateLocalEuler(const glm::vec3 &rotationEuler) {
  m_rotationEuler = rotationEuler;
  UpdateRotationMatrix();
};

template <> void Transformations::RotateLocalEuler<Axes::X>(float radians) {
  m_rotationEuler.x += radians;
  UpdateRotationMatrix();
}
template <> void Transformations::RotateLocalEuler<Axes::Y>(float radians) {
  m_rotationEuler.y += radians;
  UpdateRotationMatrix();
}
template <> void Transformations::RotateLocalEuler<Axes::Z>(float radians) {
  m_rotationEuler.z += radians;
  UpdateRotationMatrix();
}

//
// --- SCALE --------------------------------------------------------------
// --- SCALE --------------------------------------------------------------
//
void Transformations::ScaleLocal(float scalarValue) {
  m_scale.x *= scalarValue;
  m_scale.y *= scalarValue;
  m_scale.z *= scalarValue;
  UpdateScaleMatrix();
}

void Transformations::ScaleLocal(const glm::vec3 &scaleVector) {
  m_scale = scaleVector;
  UpdateScaleMatrix();
}

template <> void Transformations::ScaleLocal<Axes::X>(float scalarValue) {
  m_scale.x *= scalarValue;
  UpdateScaleMatrix();
}

template <> void Transformations::ScaleLocal<Axes::Y>(float scalarValue) {
  m_scale.y *= scalarValue;
  UpdateScaleMatrix();
}

template <> void Transformations::ScaleLocal<Axes::Z>(float scalarValue) {
  m_scale.z *= scalarValue;
  UpdateScaleMatrix();
}

//
// --- MATRIX STUFF -------------------------------------------------------
// --- MATRIX STUFF -------------------------------------------------------
//
void Transformations::UpdatePositionMatrix() {
  m_translationMatrix = glm::translate(PC_IDENTITY_MAT4, m_position);
  UpdateLocalMatrix();
}

void Transformations::UpdateRotationMatrix() {
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

void Transformations::UpdateScaleMatrix() {
  m_scaleMatrix = glm::scale(PC_IDENTITY_MAT4, m_scale);
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
};

void Transformations::UpdateLookAtDirection() {
  glm::mat4 rotScale = m_rotationMatrix * m_scaleMatrix;
  glm::vec3 initialLookAt{0.f, 0.f, -1.f}; // facing the world -Z

  m_lookAtDir =
      glm::normalize(glm::vec3(rotScale * glm::vec4(initialLookAt, 0.f)));
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
