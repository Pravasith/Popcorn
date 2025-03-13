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

  [[nodiscard]] const glm::mat4 &GetMatrix() const { return m_matrix; };

  // inline void SetPosition(glm::vec3 pos) {
  //   m_position = pos;
  //   UpdateMatrix();
  // };
  //
  // inline void RotateY(float degrees) {
  //   m_rotationEuler.y += glm::radians(degrees);
  //   UpdateMatrix();
  // };

  void UpdateMatrix();

protected:
  // glm::vec3 m_position;
  // glm::vec3 m_rotationEuler;

  glm::mat4 m_matrix = glm::mat4(1.0f);

  // glm::mat4 m_matrix =
  //     glm::rotate(glm::mat4(1.0f),
  //                 glm::radians(35.0f), // Tilt angle (experiment with
  //                 35°-45°) glm::vec3(1.0f, -1.0f, 0.0f) // Rotate around
  //                 diagonal axis
  //     );
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
