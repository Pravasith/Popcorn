#include "Transforms.h"
#include "GlobalMacros.h"

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
  m_rotationEuler.Set(rotationEuler, [&]() { UpdateRotationMatrix(); });
};

template <> void Transformations::RotateLocalEuler<Axes::X>(float radians) {
  m_rotationEuler.AddComponent<Axes::X>(radians,
                                        [&]() { UpdateRotationMatrix(); });
}
template <> void Transformations::RotateLocalEuler<Axes::Y>(float radians) {
  m_rotationEuler.AddComponent<Axes::Y>(radians,
                                        [&]() { UpdateRotationMatrix(); });
}
template <> void Transformations::RotateLocalEuler<Axes::Z>(float radians) {
  m_rotationEuler.AddComponent<Axes::Z>(radians,
                                        [&]() { UpdateRotationMatrix(); });
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
  auto rotX =
      glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.GetValue().x, {1, 0, 0});
  auto rotY =
      glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.GetValue().y, {0, 1, 0});
  auto rotZ =
      glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.GetValue().z, {0, 0, 1});

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
};

void Transformations::UpdateLookAtDirection() {
  // glm::mat4 rotMat = m_rotationMatrix;
  glm::vec3 initialLookAt{0.f, 0.f, -1.f}; // facing the world -Z

  m_lookAtDir =
      // glm::normalize(glm::vec3(rotMat * glm::vec4(initialLookAt, 0.f)));
      glm::normalize(glm::mat3(m_rotationMatrix) * initialLookAt);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
