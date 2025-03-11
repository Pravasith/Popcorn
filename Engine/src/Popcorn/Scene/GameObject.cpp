#include "GameObject.h"
#include "GlobalMacros.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

void GameObject::UpdateMatrix() {
  m_matrix = glm::translate(m_matrix, m_position);
  m_matrix = glm::rotate(m_matrix, m_rotationEuler.y, glm::vec3(0.f, 1.f, 0.f));

  // TODO: Update other tranformations (other euler rotations & scaling) as well
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
