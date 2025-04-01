#include "GameObject.h"
#include "GlobalMacros.h"
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

void GameObject::RotateX(float radians) {
  m_rotationEuler.x += radians;
  UpdateRotationMatrix();
  UpdateChildren();
}

void GameObject::RotateY(float radians) {
  m_rotationEuler.y += radians;
  UpdateRotationMatrix();
  UpdateChildren();
}

void GameObject::RotateZ(float radians) {
  m_rotationEuler.z += radians;
  UpdateRotationMatrix();
  UpdateChildren();
}

void GameObject::SetPosition(glm::vec3 pos) {
  m_position = pos;
  UpdatePositionMatrix();
  UpdateChildren();
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
