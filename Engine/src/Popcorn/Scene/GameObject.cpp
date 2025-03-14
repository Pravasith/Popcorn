#include "GameObject.h"
#include "GlobalMacros.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

constexpr glm::mat4 IDENTITY_MAT4 = glm::mat4(1.0);

void GameObject::UpdateMatrix() {
  m_matrix = glm::rotate(IDENTITY_MAT4, m_rotationEuler.z,
                         glm::vec3(0.f, 0.f, 1.f)) * // Z-axis
             glm::rotate(IDENTITY_MAT4, m_rotationEuler.y,
                         glm::vec3(0.f, 1.f, 0.f)) * // Y-axis
             glm::rotate(IDENTITY_MAT4, m_rotationEuler.x,
                         glm::vec3(1.f, 0.f, 0.f)) *    // X-axis
             glm::translate(IDENTITY_MAT4, m_position); // Translation
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
