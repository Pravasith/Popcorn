#include "GameObject.h"
#include "GlobalMacros.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

constexpr glm::mat4 IDENTITY_MAT4 = glm::mat4(1.0);

void GameObject::AddChild(GameObject *gameObj) {
  if (std::find(m_children.begin(), m_children.end(), gameObj) !=
      m_children.end()) {
    PC_WARN("GameObject already added as a child");
    return;
  }
  gameObj->m_parent = this;
  gameObj->m_transformData.m_worldMatrixNeedsUpdate = true;
  gameObj->UpdateChildrenWorldMatrixNeedsUpdateFlag();

  m_children.push_back(gameObj);
}

// void GameObject::DeleteChild(GameObject *gameObj) {
//   auto it = std::find(m_children.begin(), m_children.end(), gameObj);
//   if (it != m_children.end()) {
//     gameObj->m_parent = nullptr;
//     delete *it; // Deletes gameObj->m_children[] too (recursively)
//     m_children.erase(it);
//   } else {
//     PC_WARN("GameObject not found, hence not deleted");
//   }
// }

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
