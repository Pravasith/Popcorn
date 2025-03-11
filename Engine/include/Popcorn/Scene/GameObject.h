#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cmath>
#include <cstdint>
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
    UpdateMatrix();
  };

  inline void RotateY(float degrees) {
    m_rotationEuler.y += glm::radians(degrees);
    UpdateMatrix();
  };

  void UpdateMatrix();

private:
  glm::vec3 m_position;
  glm::vec3 m_rotationEuler;

  glm::mat4 m_matrix{1.0f}; // Center of the screen
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
