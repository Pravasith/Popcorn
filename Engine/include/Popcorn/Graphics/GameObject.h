
#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

struct SceneData {
  uint32_t sceneId;
  // TODO: make this definition
  // like ambient lights data, env map etc.
};

class GameObject {
public:
  GameObject() { PC_PRINT("CREATED", TagType::Constr, "GameObject"); };
  virtual ~GameObject() {
    PC_PRINT("DESTROYED", TagType::Destr, "GameObject");
  };

  virtual void OnAttach(const SceneData &) = 0;
  virtual void OnUpdate(const SceneData &) = 0;
  virtual void OnRender(const SceneData &) = 0;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
