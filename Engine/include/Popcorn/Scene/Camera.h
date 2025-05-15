#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct CameraData {
  float aspectRatio = 1.0f;
  // swapchainWidth / (float)swapchainHeight
};

// TODO: Complete this class
class Camera : public GameObject {
public:
  Camera(const CameraData &data) {
    // TODO: Temp data
    m_viewMatrix =
        glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));
    m_projMatrix =
        glm::perspective(glm::radians(45.0f), data.aspectRatio, 0.1f, 10.0f);

    m_projMatrix[1][1] *= -1;

    PC_PRINT("CREATED", TagType::Constr, "Camera");
  };
  ~Camera() { PC_PRINT("DESTROYED", TagType::Destr, "Camera"); };

  [[nodiscard]] const glm::mat4 &GetViewMatrix() const { return m_viewMatrix; };
  [[nodiscard]] const glm::mat4 &GetProjectionMatrix() const {
    return m_projMatrix;
  };

  virtual constexpr GameObjectTypes GetGameObjectType() const override {
    return GameObjectTypes::Camera;
  };

  virtual void OnAttach() {};
  // virtual void OnUpdate() {};
  // virtual void OnRender() {};

private:
  glm::mat4 m_viewMatrix{1.f};
  glm::mat4 m_projMatrix{1.f};
  glm::vec3 m_worldDirection{0.0f, 1.0f, 0.0f};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
