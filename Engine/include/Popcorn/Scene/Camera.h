#pragma once

#include "AnimationProperty.h"
#include <glm/ext/vector_float4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>
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

static glm::mat4 PC_BuildViewMatrix(const glm::quat &orientationQuat,
                                    const glm::vec3 &worldPos) {
  glm::mat3 rotMat = glm::mat3_cast(orientationQuat);

  glm::mat3 viewRot =
      glm::transpose(rotMat); // inverse = transpose (for rotation)

  glm::mat4 viewMat{1.0f};
  viewMat[0] = glm::vec4(viewRot[0], 0.0f);
  viewMat[1] = glm::vec4(viewRot[1], 0.0f);
  viewMat[2] = glm::vec4(viewRot[2], 0.0f);
  viewMat[3] = glm::vec4(-(viewRot * worldPos), 1.0f);

  return viewMat;
};

class Camera : public GameObject {
  //
  // TODO: Add viewProj & invViewProj matrices evaluation here for caching
  // before copying into Vulkan buffers
  //

public:
  Camera() {
    m_transformData.m_position.AddAfterMorphCbs([this]() {
      if (m_activateLookAtTarget) {
        SetLookAtDirection(glm::normalize(
            m_lookAtTarget.GetValue() -
            GetPosition())); // Note: This eventually sets rotationQuat, which
                             // eventually fires the callback below that
                             // contains UpdateViewMatrix. So view matrix is
                             // indirectly updated here too.
      } else {
        UpdateViewMatrix();
      }
    });

    m_transformData.m_rotationQuat.AddAfterMorphCbs(
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

  // When m_lookAtTarget is animated, it only updates the Quaternion orientation
  // (and eventually the rotMatrix --> lookAtDirection --> viewMatrix (via the
  // above m_rotationQuat.afterMorphCb)) when m_activateLookAtTarget is true.
  AnimationProperty<glm::vec3> *GetAnimationProperty_LookAtTarget() {
    return &m_lookAtTarget;
  }

  void ActivateLookAtTarget(bool activateLookAtTarget) {
    m_activateLookAtTarget = activateLookAtTarget;
  }

  virtual constexpr GameObjectTypes GetGameObjectType() const override {
    return GameObjectTypes::Camera;
  }

private:
  void UpdateViewMatrix() {
    // glm::vec3 lookAtPoint =
    //     // GetPosition() +
    //     GetLookAtDirection();
    // PC_WARN("LOOK-AT POINT: " << lookAtPoint.x << ", " << lookAtPoint.y << ",
    // "
    //                           << lookAtPoint.z);

    // Method 1: Numerically stable bc we use direct quaternion state
    m_viewMatrix = PC_BuildViewMatrix(GetRotationQuat(), GetPosition());

    // // Method 2: Not so numerically stable bc it uses vectors to construct
    // matrices
    // // Note: lookAt is not that expensive
    // m_viewMatrix = glm::lookAt(
    //     GetPosition(), // Camera world pos
    //     GetPosition() +
    //         GetLookAtDirection(), // Target point to look at(world pos)
    //     PC_WORLD_UP_DIR           // Up direction (world up -- Y+)
    // );
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

    PC_WARN("FOV " << m_cameraData.GetValue()[0])
  }

  virtual void OnAttach() {}
  // virtual void OnUpdate() {};
  // virtual void OnRender() {};

private:
  glm::mat4 m_viewMatrix{1.f};
  glm::mat4 m_projMatrix{1.f};

  AnimationProperty<glm::vec4> m_cameraData;
  AnimationProperty<glm::vec3> m_lookAtTarget{0.f, 0.f, 0.f};

  bool m_activateLookAtTarget = false;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
