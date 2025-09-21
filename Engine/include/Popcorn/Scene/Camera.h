#pragma once

#include "AnimationProperty.h"
#include <glm/ext/vector_float4.hpp>
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

struct CameraData {
  float fov = 45.f;
  float aspectRatio = 1.0f;
  float near = 0.1f;
  float far = 1000.0f;
};

class Camera : public GameObject {

  // TODO: Add viewProj & invViewProj matrices evaluation here for caching
  // before copying into Vulkan buffers
  //
  // TODO: Change callbacks to observer pattern

public:
  Camera() {
    m_transformData.SetCameraViewMatrixUpdateCb(
        [this]() { UpdateViewMatrix(); });
  }

public:
  void SetCameraData(const CameraData &data) {
    m_cameraData.Set(glm::vec4{
        data.fov,         // fov
        data.aspectRatio, // aspectRatio
        data.near,        // near
        data.far          // far
    });

    UpdateViewMatrix();
    UpdateProjMatrix();
  }

  [[nodiscard]] const glm::mat4 &GetViewMatrix() const { return m_viewMatrix; }
  [[nodiscard]] const glm::mat4 &GetProjectionMatrix() const {
    return m_projMatrix;
  }

  virtual constexpr GameObjectTypes GetGameObjectType() const override {
    return GameObjectTypes::Camera;
  }

private:
  void UpdateViewMatrix() {
    glm::vec3 lookAtPoint = GetPosition() + GetLookAtDirection();
    PC_WARN(lookAtPoint.x << ", " << lookAtPoint.y << ", " << lookAtPoint.z);

    // Note: lookAt is not that expensive
    m_viewMatrix = glm::lookAt(
        GetPosition(), // Camera world pos
        GetPosition() +
            GetLookAtDirection(), // Target point to look at(world pos)
        s_upDir                   // Up direction (world up -- Y+)
    );
  }

  void UpdateProjMatrix() {
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
        glm::perspectiveRH_ZO(m_cameraData.GetValue()[0], // fov
                              m_cameraData.GetValue()[1], // aspectRatio
                              m_cameraData.GetValue()[2], // near
                              m_cameraData.GetValue()[3]  // far
        );
    m_projMatrix[1][1] *= -1;
  }

  virtual void OnAttach() {}
  // virtual void OnUpdate() {};
  // virtual void OnRender() {};

private:
  glm::mat4 m_viewMatrix{1.f};
  glm::mat4 m_projMatrix{1.f};

  AnimationProperty<glm::vec4> m_cameraData;

  bool m_viewProjMatrixNeedsUpdate = false;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
