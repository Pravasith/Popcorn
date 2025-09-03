#include "GameObject.h"
#include "GlobalMacros.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <stdexcept>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

constexpr glm::mat4 IDENTITY_MAT4 = glm::mat4(1.0);

void GameObject::AddChild(GameObject *child) {
  if (child == m_parent) {
    PC_WARN(child->GetName() << " GameObject AddChild() error")
    throw std::runtime_error(
        "Game object's parent cannot be added as a child, creates a cycle");
  }

  if (std::find(m_children.begin(), m_children.end(), child) !=
      m_children.end()) {
    PC_WARN("GameObject already added as a child");
    return;
  }

  if (child->m_parent) {
    throw std::runtime_error(
        "Object already has a parent. Detach parent before adding as a child");
  }

  child->m_parent = this;
  child->m_transformData.m_worldMatrixNeedsUpdate = true;
  child->UpdateChildrenWorldMatrixNeedsUpdateFlag();

  m_children.push_back(child);
}

void GameObject::DetachChild(GameObject *child) {
  auto it = std::find(m_children.begin(), m_children.end(), child);
  if (it != m_children.end()) {
    child->m_parent = nullptr;
    m_children.erase(it);
  } else {
    PC_WARN("Child GameObject not found, hence not detached");
  }
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
