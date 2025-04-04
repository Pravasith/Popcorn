#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Camera : public GameObject {
public:
  Camera() { PC_PRINT("CREATED", TagType::Constr, "Camera"); }
  ~Camera() { PC_PRINT("DESTROYED", TagType::Destr, "Camera"); }

  virtual constexpr GameObjectTypes GetType() const {
    return GameObjectTypes::Camera;
  };

  virtual void OnAttach() {};
  virtual void OnUpdate() {};
  virtual void OnRender() {};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
