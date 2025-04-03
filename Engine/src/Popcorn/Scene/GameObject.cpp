#include "GameObject.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

constexpr glm::mat4 IDENTITY_MAT4 = glm::mat4(1.0);

void GameObject::SetParent(GameObject *gameObj) {
  if (m_parent) {
    PC_ERROR("GameObject already has a parent!", "GameObject");
    return;
  }
  m_parent = gameObj;
}

void GameObject::AddChild(GameObject *gameObj) {
  if (std::find(m_children.begin(), m_children.end(), gameObj) !=
      m_children.end()) {
    PC_WARN("GameObject already added as a child");
    return;
  }
  gameObj->m_parent = this;
  m_children.push_back(gameObj);
}

void GameObject::DeleteChild(GameObject *gameObj) {
  auto it = std::find(m_children.begin(), m_children.end(), gameObj);
  if (it != m_children.end()) {
    gameObj->m_parent = nullptr;
    delete *it;
    m_children.erase(it);
  } else {
    PC_WARN("GameObject not found, hence not deleted");
  }
}

//
//
// -----------------------------------------------------------------
// --- ROTATION ----------------------------------------------------

void GameObject::RotateEuler(float radians, Axes a) {
  switch (a) {
  case Axes::X:
    m_rotationEuler.x += radians;
    break;
  case Axes::Y:
    m_rotationEuler.y += radians;
    break;
  case Axes::Z:
    m_rotationEuler.z += radians;
    break;
  default:
    PC_WARN("Wrong axes")
    break;
  }
  UpdateRotationMatrix();
}

void GameObject::SetRotationEuler(glm::vec3 rotationEuler) {
  m_rotationEuler = rotationEuler;
  UpdateRotationMatrix();
};

void GameObject::UpdateRotationMatrix() {
  auto rotX = glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.x, {1, 0, 0});
  auto rotY = glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.y, {0, 1, 0});
  auto rotZ = glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.z, {0, 0, 1});

  switch (m_eulerOrder) {
  case EulerOrder::XYZ:
    m_rotationMatrix = rotX * rotY * rotZ; // X → Y → Z
    break;
  case EulerOrder::XZY:
    m_rotationMatrix = rotX * rotZ * rotY; // X → Z → Y
    break;
  case EulerOrder::YXZ:
    m_rotationMatrix = rotY * rotX * rotZ; // Y → X → Z
    break;
  case EulerOrder::YZX:
    m_rotationMatrix = rotY * rotZ * rotX; // Y → Z → X
    break;
  case EulerOrder::ZXY:
    m_rotationMatrix = rotZ * rotX * rotY; // Z → X → Y
    break;
  case EulerOrder::ZYX:
    m_rotationMatrix = rotZ * rotY * rotX; // Z → Y → X
    break;
  default: // XYZ
    m_rotationMatrix = rotX * rotY * rotZ;
    break;
  }

  UpdateLocalMatrix();
}

//
//
// -----------------------------------------------------------------
// --- TRANSLATION -------------------------------------------------

void GameObject::Translate(float signedDistance, Axes axis) {
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

void GameObject::SetPosition(glm::vec3 pos) {
  m_position = pos;
  UpdatePositionMatrix();
}

void GameObject::UpdatePositionMatrix() {
  m_translationMatrix = glm::translate(PC_IDENTITY_MAT4, m_position);
  UpdateLocalMatrix();
}

//
//
// -----------------------------------------------------------------
// --- SCALE -------------------------------------------------------

template <> void GameObject::Scale<Axes::X>(float scalarValue) {
  m_scale.x *= scalarValue;
  UpdateScaleMatrix();
};

template <> void GameObject::Scale<Axes::Y>(float scalarValue) {
  m_scale.y *= scalarValue;
  UpdateScaleMatrix();
};

template <> void GameObject::Scale<Axes::Z>(float scalarValue) {
  m_scale.z *= scalarValue;
  UpdateScaleMatrix();
};

void GameObject::Scale(float scalarValue) {
  m_scale.x *= scalarValue;
  m_scale.y *= scalarValue;
  m_scale.z *= scalarValue;
  UpdateScaleMatrix();
};

void GameObject::SetScale(glm::vec3 scale) {
  m_scale = scale;
  UpdateScaleMatrix();
};

void GameObject::UpdateScaleMatrix() {
  m_scaleMatrix = glm::scale(PC_IDENTITY_MAT4, m_scale);
  UpdateLocalMatrix();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
