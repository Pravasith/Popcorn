#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Light : public GameObject {
public:
  Light() { PC_PRINT("CREATED", TagType::Constr, "Light"); }
  ~Light() { PC_PRINT("DESTROYED", TagType::Destr, "Light"); }

  virtual constexpr GameObjectTypes GetType() const {
    return GameObjectTypes::Light;
  };

  virtual void OnAttach() {};
  virtual void OnUpdate() {};
  virtual void OnRender() {};

private:
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
