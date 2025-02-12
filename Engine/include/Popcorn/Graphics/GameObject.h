
#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class GameObject {
public:
  GameObject() { PC_PRINT("CREATED", TagType::Constr, "GameObject"); };
  virtual ~GameObject() {
    PC_PRINT("DESTROYED", TagType::Destr, "GameObject");
  };

  virtual void OnAttach() {};
  virtual void OnUpdate() = 0;
  virtual void OnRender() = 0;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
