#pragma once

#include "GameObject.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Scene {
public:
  Scene() { PC_PRINT("CREATED", TagType::Constr, "Scene"); };
  virtual ~Scene() { PC_PRINT("DESTROYED", TagType::Destr, "Scene"); };

  virtual void
  Add(GameObject *node) = 0; // GameObject can be a lightweight wrapper
  virtual void Update() = 0;
  virtual void Render() = 0;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
