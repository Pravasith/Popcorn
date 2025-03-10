#pragma once

#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class MaterialSystem {
public:
  MaterialSystem(MaterialData &matData) {
    PC_PRINT("CREATED", TagType::Constr, "MaterialSystem")
  };
  ~MaterialSystem() { PC_PRINT("DESTROYED", TagType::Destr, "MaterialSystem") };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
