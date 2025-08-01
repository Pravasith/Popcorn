#pragma once

#include "GlobalMacros.h"
#include "MathConstants.h"
#include "Popcorn/Core/Base.h"
#include <cmath>
#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct SceneData {
  uint32_t sceneId;
  // TODO: make this definition
  // like ambient lights data, env map etc.
};

enum class GameObjectTypes {
  None = 0,
  Mesh,
  Camera,
  Empty,
  Light,
};

enum class Axes { X = 1, Y, Z };

enum class EulerOrder {
  XYZ = 1,
  XZY,
  YXZ,
  YZX,
  ZXY,
  ZYX,
};

enum class Transforms { Translate = 1, Rotate, Scale, Shear, Reflect };

class GameObject {
public:
  std::string name;

public:
  GameObject() { PC_PRINT("CREATED", TagType::Constr, "GameObject"); }
  virtual ~GameObject() {
    for (auto *child : m_children) {
      child->m_parent = nullptr;
      delete child; // Recursive
    }
    m_children.clear();

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

  // MOVE ASSIGNMENT
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

  virtual constexpr GameObjectTypes GetGameObjectType() const = 0;

  virtual void OnUpdate() {};
  virtual void OnRender() {};
  virtual void OnEvent() {};

  [[nodiscard]] const std::vector<GameObject *> GetChildren() const {
    return m_children;
  }

  void SetParent(GameObject *gameObj);
  void RemoveParent();
  void AddChild(GameObject *gameObj);
  void DeleteChild(GameObject *gameObj);

  // Translation
  void Translate(float signedDistance, Axes axis);
  void SetPosition(glm::vec3 pos);
  [[nodiscard]] const glm::vec3 &GetPosition() const { return m_position; };

  // Rotation
  void SetEulerOrder(EulerOrder order) { m_eulerOrder = order; };
  template <Axes T> void RotateEuler(float radians);
  void SetRotationEuler(glm::vec3 rotationEuler);

  // Scale
  template <Axes T> void ScaleAlongAxis(float scalarValue);
  void ScaleUniformly(float scalarValue);
  void ScaleByValue(glm::vec3 scaleVector);

  [[nodiscard]] const glm::mat4 &GetLocalMatrix() const {
    return m_localMatrix;
  }

  void SetLocalMatrix(const glm::mat4 &mat) {
    m_localMatrix = mat;
    m_worldMatrixNeedsUpdate = true;
    UpdateChildrenWorldMatrixNeedsUpdateFlag();
  };

  [[nodiscard]] const glm::mat4 &GetWorldMatrix() {
    if (m_worldMatrixNeedsUpdate) {
      UpdateWorldMatrix(); // Recursive -- internally calls child's
                           // GetWorldMatrix()
    };
    return m_worldMatrix;
  }

  [[nodiscard]] const glm::vec3 &GetLookAtDirection() const {
    return m_lookAtDir;
  };

private:
  void UpdatePositionMatrix();
  void UpdateRotationMatrix();
  void UpdateScaleMatrix();
  void UpdateLookAtDirection();

  void UpdateLocalMatrix() {
    m_localMatrix = m_translationMatrix * m_rotationMatrix * m_scaleMatrix;
    m_worldMatrixNeedsUpdate = true;

    UpdateChildrenWorldMatrixNeedsUpdateFlag();
  };

  void UpdateWorldMatrix() {
    if (m_parent) {
      m_worldMatrix = m_parent->GetWorldMatrix() * m_localMatrix;
    } else {
      m_worldMatrix =
          m_localMatrix; // Root object uses local matrix as world matrix
    }

    m_worldMatrixNeedsUpdate = false;
  };

  void UpdateChildrenWorldMatrixNeedsUpdateFlag() {
    for (auto *child : m_children) {
      if (child) {
        child->m_worldMatrixNeedsUpdate = true;
        child->UpdateChildrenWorldMatrixNeedsUpdateFlag();
      }
    }
  }

protected:
  GameObject *m_parent = nullptr;
  std::vector<GameObject *> m_children;

  EulerOrder m_eulerOrder = EulerOrder::XYZ;

  glm::vec3 m_position{0, 0, 0};
  glm::vec3 m_rotationEuler{0, 0, 0};
  glm::vec3 m_scale{1, 1, 1};

  glm::mat4 m_translationMatrix = PC_IDENTITY_MAT4;
  glm::mat4 m_rotationMatrix = PC_IDENTITY_MAT4;
  glm::mat4 m_scaleMatrix = PC_IDENTITY_MAT4;

  glm::mat4 m_localMatrix = PC_IDENTITY_MAT4; // Local -> Parent
  glm::mat4 m_worldMatrix = PC_IDENTITY_MAT4; // Local -> World

  glm::vec3 m_lookAtDir{0.f, 0.f, -1.f}; // towards the screen

  bool m_worldMatrixNeedsUpdate = false;

protected:
  static constexpr glm::vec3 s_upDir{0.f, 1.f, 0.f}; // +Y up
};

template <typename T>
concept GameObjectType = std::is_base_of<GameObject, T>::value;

//
//
// UTIL FUNCTIONS -------------------------------------------------------------
// UTIL FUNCTIONS -------------------------------------------------------------
// UTIL FUNCTIONS -------------------------------------------------------------
//
template <GameObjectType T>
bool PC_ValidateAndAddGameObject(T *gameObject, std::vector<T *> &gameObjects) {
  auto ptr = std::find(gameObjects.begin(), gameObjects.end(), gameObject);

  if (ptr != gameObjects.end()) {
    PC_WARN("GameObject already exists in the gameObject library!")
    return false;
  };

  gameObjects.emplace_back(gameObject);
  return true;
};

// Doesn't delete, just erases
template <GameObjectType T>
bool PC_ValidateAndRemoveGameObject(T *gameObject,
                                    std::vector<T *> &gameObjects) {
  auto ptr = std::find(gameObjects.begin(), gameObjects.end(), gameObject);

  if (ptr == gameObjects.end()) {
    PC_WARN("GameObject not found!")
    return false;
  };

  gameObjects.erase(ptr);
  return true;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
