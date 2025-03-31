#pragma once

#include "GlobalMacros.h"
#include "MathConstants.h"
#include "Popcorn/Core/Assert.h"
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
    // Recursively deletes sub-children
    for (auto &child : m_children) {
      child->m_parent = nullptr;
      delete child;
      child = nullptr;
    }

    PC_PRINT("DESTROYED", TagType::Destr, "GameObject");
  };

  virtual constexpr GameObjectTypes GetType() const = 0;

  virtual void OnAttach() = 0;
  virtual void OnUpdate() = 0;
  virtual void OnRender() = 0;

  void SetParent(GameObject *gameObj) {
    PC_ASSERT(!m_parent, "Game object already has a parent!");
    m_parent = gameObj;
  };

  void AddChild(GameObject *gameObj) {
    if (std::find(m_children.begin(), m_children.end(), gameObj) !=
        m_children.end()) {
      PC_WARN("GameObject " << gameObj << "already added as a child")
    } else {
      gameObj->m_parent = this;
      m_children.push_back(gameObj);
    };
  };

  void DeleteChild(GameObject *gameObj) {
    auto it = std::find(m_children.begin(), m_children.end(), gameObj);
    if (it != m_children.end()) {
      gameObj->m_parent = nullptr;
      m_children.erase(it);
      delete gameObj;
      gameObj = nullptr;
    } else {
      PC_WARN("GameObject " << gameObj << "not found, hence not deleted")
    };
  };

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

  inline void SetPosition(glm::vec3 pos) {
    m_position = pos;
    UpdatePositionMatrix();
  }

  const glm::mat4 &GetMatrix() const { return m_matrix; }
  void SetMatrix(glm::mat4 matrix) { m_matrix = matrix; };

private:
  void UpdatePositionMatrix() {
    m_translationMatrix = glm::translate(PC_IDENTITY_MAT4, m_position);
    m_matrix = m_translationMatrix * m_rotationMatrix;
  }

  void UpdateRotationMatrix() {
    m_rotationMatrix =
        glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.x, {1, 0, 0}) *
        glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.y, {0, 1, 0}) *
        glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.z, {0, 0, 1});
    m_matrix = m_rotationMatrix * m_translationMatrix;
  }

protected:
  GameObject *m_parent = nullptr;
  std::vector<GameObject *> m_children;

  glm::vec3 m_position = {0, 0, 0};
  glm::vec3 m_rotationEuler = {0, 0, 0};
  glm::mat4 m_translationMatrix = PC_IDENTITY_MAT4;
  glm::mat4 m_rotationMatrix = PC_IDENTITY_MAT4;
  glm::mat4 m_matrix = PC_IDENTITY_MAT4;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
