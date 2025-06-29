#include "GameObject.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

constexpr glm::mat4 IDENTITY_MAT4 = glm::mat4(1.0);

// Keeps transform
void GameObject::SetParent(GameObject *gameObj) {
  if (m_parent) {
    PC_ERROR("GameObject already has a parent!", "GameObject");
    return;
  }

  m_parent = gameObj;
  m_worldMatrixNeedsUpdate = true;
  UpdateChildrenWorldMatrixNeedsUpdateFlag();
}

void GameObject::RemoveParent() {
  if (m_parent != nullptr) {
    m_parent = nullptr;
    m_worldMatrixNeedsUpdate = true;
    UpdateChildrenWorldMatrixNeedsUpdateFlag();
  };
};

void GameObject::AddChild(GameObject *gameObj) {
  if (std::find(m_children.begin(), m_children.end(), gameObj) !=
      m_children.end()) {
    PC_WARN("GameObject already added as a child");
    return;
  }
  gameObj->m_parent = this;
  gameObj->m_worldMatrixNeedsUpdate = true;
  gameObj->UpdateChildrenWorldMatrixNeedsUpdateFlag();

  m_children.push_back(gameObj);
}

void GameObject::DeleteChild(GameObject *gameObj) {
  auto it = std::find(m_children.begin(), m_children.end(), gameObj);
  if (it != m_children.end()) {
    gameObj->m_parent = nullptr;
    delete *it; // Deletes gameObj->m_children[] too (recursively)
    m_children.erase(it);
  } else {
    PC_WARN("GameObject not found, hence not deleted");
  }
}

//
//
// -----------------------------------------------------------------
// --- ROTATION ----------------------------------------------------

template <> void GameObject::RotateEuler<Axes::X>(float radians) {
  m_rotationEuler.x += radians;
  UpdateRotationMatrix();
}

template <> void GameObject::RotateEuler<Axes::Y>(float radians) {
  m_rotationEuler.y += radians;
  UpdateRotationMatrix();
}

template <> void GameObject::RotateEuler<Axes::Z>(float radians) {
  m_rotationEuler.z += radians;
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
//
template <> void GameObject::ScaleAlongAxis<Axes::X>(float scalarValue) {
  m_scale.x *= scalarValue;
  UpdateScaleMatrix();
};

template <> void GameObject::ScaleAlongAxis<Axes::Y>(float scalarValue) {
  m_scale.y *= scalarValue;
  UpdateScaleMatrix();
};

template <> void GameObject::ScaleAlongAxis<Axes::Z>(float scalarValue) {
  m_scale.z *= scalarValue;
  UpdateScaleMatrix();
};

void GameObject::ScaleUniformly(float scalarValue) {
  m_scale.x *= scalarValue;
  m_scale.y *= scalarValue;
  m_scale.z *= scalarValue;
  UpdateScaleMatrix();
};

void GameObject::ScaleByValue(glm::vec3 scaleVector) {
  m_scale = scaleVector;
  UpdateScaleMatrix();
};

void GameObject::UpdateScaleMatrix() {
  m_scaleMatrix = glm::scale(PC_IDENTITY_MAT4, m_scale);
  UpdateLookAtDirection();
  UpdateLocalMatrix();
};

//
//
// -----------------------------------------------------------------
// --- LOOK AT DIRECTION -------------------------------------------
//
void GameObject::UpdateLookAtDirection() {
  glm::mat4 rotScale = m_rotationMatrix * m_scaleMatrix;
  glm::vec3 initialLookAt{0.f, 0.f, -1.f}; // facing the world -Z

  m_lookAtDir =
      glm::normalize(glm::vec3(rotScale * glm::vec4(initialLookAt, 0.f)));
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
