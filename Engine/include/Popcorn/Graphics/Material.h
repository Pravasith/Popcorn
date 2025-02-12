#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Material {
public:
  Material() { PC_PRINT("CREATED", TagType::Constr, "Material.h"); };
  ~Material() { PC_PRINT("DESTROYED", TagType::Destr, "Material.h"); };

  // Material data - shaders
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
