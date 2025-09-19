#pragma once

#include <glm/gtc/quaternion.hpp>
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

class Camera : public GameObject {
public:
  struct CameraData {
    float fov = 45.f;
    float aspectRatio = 1.0f;
    float near = 0.1f;
    float far = 1000.0f;
  };

public:
  Camera() = default;

  void SetCameraData(const CameraData &data) {
    m_cameraData = data;

    /**
     * Projection matrix info from learnOpenGl.com
     * -----------------------------------------------------------------------
     * 1. The proj matrix maps a given camera frustum range from view space to
     *    clip space.
     * 2. It also manipulates the W coord of the view space position in such a
     *    way that the further away vertices (not pixels!) have a bigger value
     *    so that during perspective division, they (further away vertices)
     *    shrink their coords more!
     * */
    m_projMatrix =
        glm::perspectiveRH_ZO(m_cameraData.fov, m_cameraData.aspectRatio,
                              m_cameraData.near, m_cameraData.far);
    m_projMatrix[1][1] *= -1;

    UpdateViewMatrix();
  }

  [[nodiscard]] const glm::mat4 &GetViewMatrix() const { return m_viewMatrix; }
  [[nodiscard]] const glm::mat4 &GetProjectionMatrix() const {
    return m_projMatrix;
  }

  void UpdateViewMatrix() {
    m_viewMatrix = glm::lookAt(
        GetPosition(), // Camera world pos
        GetPosition() +
            GetLookAtDirection(), // Target point to look at(world pos)
        s_upDir                   // Up direction (world up -- Y+)
    );

    // m_projMatrix = glm::perspectiveRH_ZO(glm::radians(45.0f),
    // data.aspectRatio,
    //                                      data.near, data.far);
    // m_projMatrix[1][1] *= -1;
  }

  virtual constexpr GameObjectTypes GetGameObjectType() const override {
    return GameObjectTypes::Camera;
  }

  virtual void OnAttach() {}
  // virtual void OnUpdate() {};
  // virtual void OnRender() {};

private:
  glm::mat4 m_viewMatrix{1.f};
  glm::mat4 m_projMatrix{1.f};

  CameraData m_cameraData;

  bool m_viewProjMatrixNeedsUpdate = false;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
