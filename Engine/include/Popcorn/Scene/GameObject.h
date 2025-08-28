#pragma once

#include "GlobalMacros.h"
#include "MathConstants.h"
#include "Popcorn/Core/Base.h"
#include "Transforms.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
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

class GameObject {
public:
  GameObject() {
    // PC_PRINT("CREATED", TagType::Constr, "GameObject");
  }
  virtual ~GameObject() noexcept {
    for (auto *child : m_children) {
      child->m_parent = nullptr;
      delete child; // Recursive
    }
    m_children.clear();
    // PC_PRINT("DESTROYED", TagType::Destr, "GameObject");
  };

  virtual constexpr GameObjectTypes GetGameObjectType() const = 0;
  [[nodiscard]] const std::vector<GameObject *> &GetChildren() const {
    return m_children;
  }

  void AddChild(GameObject *child);
  void DetachChild(GameObject *child);

  // TODO: Design this properly
  virtual void OnUpdate() {};
  virtual void OnRender() {};
  virtual void OnEvent() {};

  [[nodiscard]] const glm::vec3 &GetPosition() const {
    return m_transformData.m_position.GetValue();
  }
  [[nodiscard]] const glm::mat4 &GetLocalMatrix() const {
    return m_transformData.m_localMatrix;
  }

  //
  // --- Translation -------------------------------------------------------
  template <Axes T> void TranslateLocal(float signedDistance) {
    m_transformData.TranslateLocal<T>(signedDistance);
  }
  void TranslateLocal(const glm::vec3 &pos) {
    m_transformData.TranslateLocal(pos);
  }

  //
  // --- Rotation ----------------------------------------------------------
  void SetEulerOrder(EulerOrder order) { m_transformData.SetEulerOrder(order); }
  void RotateLocalEuler(const glm::vec3 &rotationEuler) {
    m_transformData.RotateLocalEuler(rotationEuler);
  }
  template <Axes T> void RotateLocalEuler(float radians) {
    m_transformData.RotateLocalEuler<T>(radians);
  }

  //
  // --- Scale -------------------------------------------------------------
  template <Axes T> void ScaleLocal(float scalarValue) {
    m_transformData.ScaleLocal<T>(scalarValue);
  };
  void ScaleLocal(float scalarValue) {
    m_transformData.ScaleLocal(scalarValue);
  };
  void ScaleLocal(glm::vec3 scaleVector) {
    m_transformData.ScaleLocal(scaleVector);
  };

  //
  // Warning: only to be used in GltfLoader(internally). Shouldn't be used
  // directly by the user. Unless the user knows what they're doing.
  void SetLocalMatrix(const glm::mat4 &mat) {
    m_transformData.SetLocalMatrix(mat);
    UpdateChildrenWorldMatrixNeedsUpdateFlag();
  };

  //
  // GetWorldMatrix() mutates internal cache.
  [[nodiscard]] const glm::mat4 &GetWorldMatrix() {
    glm::mat4 parentWorldMatrix = PC_IDENTITY_MAT4;
    if (m_parent) {
      parentWorldMatrix = m_parent->GetWorldMatrix();
    }

    if (m_transformData.m_worldMatrixNeedsUpdate) {
      m_transformData.UpdateWorldMatrix(parentWorldMatrix);
    }

    return m_transformData.m_worldMatrix;
  }

  void UpdateLocalMatrix() {
    m_transformData.UpdateLocalMatrix();
    UpdateChildrenWorldMatrixNeedsUpdateFlag();
  };

  [[nodiscard]] const glm::vec3 &GetLookAtDirection() const {
    return m_transformData.m_lookAtDir;
  };

  void UpdateChildrenWorldMatrixNeedsUpdateFlag() {
    for (auto *child : m_children) {
      if (child) {
        child->m_transformData.m_worldMatrixNeedsUpdate = true;
        child->UpdateChildrenWorldMatrixNeedsUpdateFlag();
      }
    }
  }

  void SetName(const std::string &name) {
    if (m_name.length() != 0) {
      throw std::runtime_error("Object can only be named once");
    }
    m_name = name;
  }
  const std::string &GetName() const {
    if (m_name.length() == 0) {
      throw std::runtime_error("GameObject name not set");
    }
    return m_name;
  }

public:
  // Prevent accidental copies (dangerous due to raw pointers)
  // TODO: Add a "Clone" method later
  GameObject(const GameObject &) = delete;
  GameObject &operator=(const GameObject &) = delete;
  GameObject(GameObject &&other) = delete;
  GameObject &operator=(GameObject &&other) = delete;

protected:
  std::string m_name;
  GameObject *m_parent = nullptr;
  std::vector<GameObject *> m_children;

  Transformations m_transformData;

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
}

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
}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
