#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

// Like Blender's "Empty" object. Only has transforms and parent-child relation
class Empty : public GameObject {
public:
  Empty() {
    // PC_PRINT("CREATED", TagType::Constr, "Empty");
  }
  ~Empty() {
    // PC_PRINT("DESTROYED", TagType::Destr, "Empty");
  }

  virtual constexpr GameObjectTypes GetGameObjectType() const override {
    return GameObjectTypes::Empty;
  };

  virtual void OnAttach() {};
  // virtual void OnUpdate() {};
  // virtual void OnRender() {};
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
