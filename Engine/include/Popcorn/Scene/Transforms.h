#pragma once

#include "AnimationProperty.h"
#include "GlobalMacros.h"
#include "MathConstants.h"
#include <functional>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// clang-format off
// enum class Transforms { Translate = 1, Rotate, Scale, Shear, Reflect };
enum class EulerOrder { XYZ = 1, XZY, YXZ, YZX, ZXY, ZYX };
// clang-format on

class Transformations {
public:
  // --- positioning ----------------------------------------------------------
  void TranslateLocal(const glm::vec3 &targetPos);
  template <Axes T> void TranslateLocal(float signedDistance);

  // --- rotations ------------------------------------------------------------
  //
  // Euler
  void SetEulerOrder(EulerOrder order) { m_eulerOrder = order; }
  void RotateLocalEuler(const glm::vec3 &rotationEuler);
  template <Axes T> void RotateLocalEuler(float radians);

  // Quat
  void RotateLocalQuat(const glm::quat &dq) {
    m_rotationQuat.Set(glm::normalize(dq * m_rotationQuat.GetValue()),
                       [&]() { UpdateRotationMatrix(); });
  }
  void SetRotationQuat(const glm::quat &q) {
    m_rotationQuat.Set(glm::normalize(q), [&]() { UpdateRotationMatrix(); });
  }

  // --- scaling --------------------------------------------------------------
  template <Axes T> void ScaleLocal(float scalarValue);
  void ScaleLocal(float scalarValue);
  void ScaleLocal(const glm::vec3 &scaleVector);

  // --- matrix stuff ---------------------------------------------------------
  void SetLocalMatrix(const glm::mat4 &mat); // only for GltfLoader

  //
  void UpdatePositionMatrix();
  void UpdateRotationMatrix();
  void UpdateScaleMatrix();
  void UpdateLocalMatrix();
  void UpdateWorldMatrix(const glm::mat4 &parentWorldMatrix);

  // --- lookAt stuff ---------------------------------------------------------
  void UpdateLookAtDirection();
  void SetLookAtDirection(glm::vec3 &&);

public:
  Transformations() {
    m_position.SetAfterMorphCb([this] { UpdatePositionMatrix(); });
    m_rotationQuat.SetAfterMorphCb([this] { UpdateRotationMatrix(); });
    m_scale.SetAfterMorphCb([this] { UpdateScaleMatrix(); });
  }
  ~Transformations() = default;

  Transformations(const Transformations &) = delete;
  Transformations &operator=(const Transformations &) = delete;
  Transformations(Transformations &&other) = delete;
  Transformations &operator=(Transformations &&other) = delete;

public:
  template <typename F>
  void SetCameraViewMatrixUpdateCb(F &&cameraViewMatrixUpdate_Cb) {
    m_cameraViewMatrixUpdate_Cb = std::forward<F>(cameraViewMatrixUpdate_Cb);
  }

  template <typename F>
  void
  SetGameObjChildWorldMatUpdateFlagCb(F &&gameObjChildWorldMatUpdateFlag_Cb) {
    m_gameObjChildWorldMatUpdateFlag_Cb =
        std::forward<F>(gameObjChildWorldMatUpdateFlag_Cb);
  }

private:
  friend class GameObject;

  // --- animations -----------------------------------------------------------

  // Warning: To be modified by the GameObject class only
  [[nodiscard]] AnimationProperty<glm::vec3> *
  GetAnimationProperty_Pos() noexcept {
    return &m_position;
  }

  // Warning: To be modified by the GameObject class only
  [[nodiscard]] AnimationProperty<glm::quat> *
  GetAnimationProperty_RotQuat() noexcept {
    return &m_rotationQuat;
  }

  // Warning: To be modified by the GameObject class only
  [[nodiscard]] AnimationProperty<glm::vec3> *
  GetAnimationProperty_Scale() noexcept {
    return &m_scale;
  }

public:
  EulerOrder m_eulerOrder = EulerOrder::XYZ;

  AnimationProperty<glm::vec3> m_position{0.f, 0.f, 0.f};
  AnimationProperty<glm::quat> m_rotationQuat{1.0f, 0.0f, 0.0f, 0.0f};
  AnimationProperty<glm::vec3> m_scale{1.f, 1.f, 1.f};

  glm::mat4 m_translationMatrix = PC_IDENTITY_MAT4;
  glm::mat4 m_rotationMatrix = PC_IDENTITY_MAT4;
  glm::mat4 m_scaleMatrix = PC_IDENTITY_MAT4;

  glm::mat4 m_localMatrix = PC_IDENTITY_MAT4; // Local -> Parent
  glm::mat4 m_worldMatrix = PC_IDENTITY_MAT4; // Local -> World

  glm::vec3 m_lookAtDir{0.f, 0.f, -1.f}; // towards the screen

  bool m_worldMatrixNeedsUpdate = false;

  // TODO: Change callbacks to observer pattern
  // Callbacks (for game objects -- camera, lights etc.)
  std::function<void()> m_gameObjChildWorldMatUpdateFlag_Cb = nullptr;
  std::function<void()> m_cameraViewMatrixUpdate_Cb = nullptr;
};

//
template <> void Transformations::TranslateLocal<Axes::X>(float);
template <> void Transformations::TranslateLocal<Axes::Y>(float);
template <> void Transformations::TranslateLocal<Axes::Z>(float);

template <> void Transformations::RotateLocalEuler<Axes::X>(float);
template <> void Transformations::RotateLocalEuler<Axes::Y>(float);
template <> void Transformations::RotateLocalEuler<Axes::Z>(float);

template <> void Transformations::ScaleLocal<Axes::X>(float);
template <> void Transformations::ScaleLocal<Axes::Y>(float);
template <> void Transformations::ScaleLocal<Axes::Z>(float);

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
