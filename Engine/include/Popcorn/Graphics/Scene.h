#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Scene {
public:
  Scene() { PC_PRINT("CREATED", TagType::Constr, "Scene"); };
  virtual ~Scene() { PC_PRINT("DESTROYED", TagType::Destr, "Scene"); };

  virtual void Render() = 0;
  virtual void
  Add(void *anyGameObject) = 0; // GameObject can be a lightweight wrapper
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
