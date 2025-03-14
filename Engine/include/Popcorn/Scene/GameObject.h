#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cmath>
#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct SceneData {
  uint32_t sceneId;
  // TODO: make this definition
  // like ambient lights data, env map etc.
};

enum class GameObjectTypes {
  Mesh = 1,
  // Camera,
};

class GameObject {
public:
  GameObject() { PC_PRINT("CREATED", TagType::Constr, "GameObject"); };
  virtual ~GameObject() {
    PC_PRINT("DESTROYED", TagType::Destr, "GameObject");
  };

  virtual constexpr GameObjectTypes GetType() const = 0;

  virtual void OnAttach() = 0;
  virtual void OnUpdate() = 0;
  virtual void OnRender() = 0;

  inline void SetPosition(glm::vec3 pos) {
    m_position = pos;
    UpdatePositionMatrix();
  }

  inline void RotateX(float radians) {
    m_rotationEuler.x += radians;
    UpdateRotationMatrix();
  }

  inline void RotateY(float radians) {
    m_rotationEuler.y += radians;
    UpdateRotationMatrix();
  }

  inline void RotateZ(float radians) {
    m_rotationEuler.z += radians;
    UpdateRotationMatrix();
  }

  const glm::mat4 &GetMatrix() const { return m_matrix; }

  inline void RotateAroundWorldYAxis(float radians) {

    // Step 1: Calculate the current position relative to the world origin
    glm::vec3 positionRelativeToOrigin = m_position;

    // Step 2: Apply the world Y-axis rotation to the position
    glm::mat4 rotationMatrix =
        glm::rotate(glm::mat4(1.0f), radians, glm::vec3(0.0f, 1.0f, 0.0f));

    // Rotate the position (ignoring the rotation of the object itself)
    positionRelativeToOrigin =
        glm::vec3(rotationMatrix * glm::vec4(positionRelativeToOrigin, 1.0f));

    // Step 3: Update the object's position to the new rotated position
    m_position = positionRelativeToOrigin;

    // Step 4: Update the orientation (rotation) of the object
    // We also need to apply the world Y-axis rotation to the object's rotation
    m_rotationMatrix = rotationMatrix * m_rotationMatrix;

    // Step 5: Recalculate the matrix based on the new position and updated
    // rotation
    UpdatePositionMatrix();
  };

private:
  void UpdatePositionMatrix() {
    m_translationMatrix = glm::translate(glm::mat4(1.0f), m_position);
    UpdateFinalMatrix();
  }

  void UpdateRotationMatrix() {
    m_rotationMatrix =
        glm::rotate(glm::mat4(1.0f), m_rotationEuler.x, {1, 0, 0}) *
        glm::rotate(glm::mat4(1.0f), m_rotationEuler.y, {0, 1, 0}) *
        glm::rotate(glm::mat4(1.0f), m_rotationEuler.z, {0, 0, 1});
    UpdateFinalMatrix();
  }

  void UpdateFinalMatrix() {
    // Apply rotation first, then translation
    m_matrix = m_translationMatrix * m_rotationMatrix;
  }

protected:
  glm::vec3 m_position = {0, 0, 0};
  glm::vec3 m_rotationEuler = {0, 0, 0};
  glm::mat4 m_translationMatrix = glm::mat4(1.0f);
  glm::mat4 m_rotationMatrix = glm::mat4(1.0f);
  glm::mat4 m_matrix = glm::mat4(1.0f);
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
