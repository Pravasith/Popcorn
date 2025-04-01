#pragma once

#include "GlobalMacros.h"
#include "MathConstants.h"
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
  GameObject() { PC_PRINT("CREATED", TagType::Constr, "GameObject"); }
  virtual ~GameObject() {

    // Safely delete children
    for (auto child : m_children) {
      child->m_parent = nullptr;
      delete child; // Calls child's destructor, ensuring it cleans itself up
    }
    m_children.clear(); // Avoids dangling pointers

    PC_PRINT("DESTROYED", TagType::Destr, "GameObject");
  }

  // Prevent accidental copies (dangerous due to raw pointers)
  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;

  // MOVE CONSTRUCTOR
  GameObject(GameObject &&other) noexcept {
    m_parent = other.m_parent;
    m_children = std::move(other.m_children);
    m_position = other.m_position;
    m_rotationEuler = other.m_rotationEuler;
    m_translationMatrix = other.m_translationMatrix;
    m_rotationMatrix = other.m_rotationMatrix;
    m_localMatrix = other.m_localMatrix;
    m_worldMatrix = other.m_worldMatrix;

    other.m_parent = nullptr;
    other.m_children.clear();
  }

  // COPY CONSTRUCTOR
  GameObject &operator=(GameObject &&other) noexcept {
    if (this != &other) {
      for (auto child : m_children)
        delete child;
      m_children.clear();

      m_parent = other.m_parent;
      m_children = std::move(other.m_children);
      m_position = other.m_position;
      m_rotationEuler = other.m_rotationEuler;
      m_translationMatrix = other.m_translationMatrix;
      m_rotationMatrix = other.m_rotationMatrix;
      m_localMatrix = other.m_localMatrix;
      m_worldMatrix = other.m_worldMatrix;

      other.m_parent = nullptr;
      other.m_children.clear();
    }
    return *this;
  }

  virtual constexpr GameObjectTypes GetType() const = 0;

  virtual void OnAttach() = 0;
  virtual void OnUpdate() = 0;
  virtual void OnRender() = 0;

  void SetParent(GameObject *gameObj);
  void AddChild(GameObject *gameObj);
  void DeleteChild(GameObject *gameObj);

  void SetPosition(glm::vec3 pos);

  void RotateX(float radians);
  void RotateY(float radians);
  void RotateZ(float radians);

  [[nodiscard]] const glm::mat4 &GetLocalMatrix() const {
    return m_localMatrix;
  }

  void SetLocalMatrix(glm::mat4 mat) {
    m_localMatrix = mat;
    UpdateChildren();
  }

private:
  void UpdatePositionMatrix() {
    m_translationMatrix = glm::translate(PC_IDENTITY_MAT4, m_position);
    m_localMatrix = m_translationMatrix * m_rotationMatrix;
  }

  void UpdateRotationMatrix() {
    m_rotationMatrix =
        glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.x, {1, 0, 0}) *
        glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.y, {0, 1, 0}) *
        glm::rotate(PC_IDENTITY_MAT4, m_rotationEuler.z, {0, 0, 1});
    m_localMatrix = m_rotationMatrix * m_translationMatrix;
  }

  void UpdateChildren() {
    for (auto *child : m_children) {
      if (child) {
        child->m_worldMatrixNeedsUpdate = true;
        child->UpdateChildren();
      }
    }
  }

protected:
  GameObject *m_parent = nullptr;
  std::vector<GameObject *> m_children;

  glm::vec3 m_position = {0, 0, 0};
  glm::vec3 m_rotationEuler = {0, 0, 0};
  glm::mat4 m_translationMatrix = PC_IDENTITY_MAT4;
  glm::mat4 m_rotationMatrix = PC_IDENTITY_MAT4;

  glm::mat4 m_localMatrix = PC_IDENTITY_MAT4;
  glm::mat4 m_worldMatrix = PC_IDENTITY_MAT4;

  mutable bool m_worldMatrixNeedsUpdate = false;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
