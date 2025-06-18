#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "GameObject.h"
#include "GlobalMacros.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct CameraData {
  float aspectRatio = 1.0f;
  float near = 0.1f;
  float far = 1000.0f;
};

// TODO: Complete this class
class Camera : public GameObject {
public:
  Camera(const CameraData &data = {1.0f, .1f, 2000.0f}) {
    // TODO: Hardcoding for now, make it dynamic later

    float x = 400.0f;
    m_viewMatrix = glm::lookAt(
        glm::vec3(x, x, x),          // Eye position(camera/object world pos)
        glm::vec3(0.0f, 0.0f, 0.0f), // Target point to look at(world pos)
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction (world up -- Y+)
    );
    m_projMatrix = glm::perspectiveRH_ZO(glm::radians(45.0f), data.aspectRatio,
                                         data.near, data.far);
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
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
