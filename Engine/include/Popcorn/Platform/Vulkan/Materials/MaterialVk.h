#pragma once

#include "GlobalMacros.h"
#include "Material.h"
#include "Popcorn/Core/Base.h"

//
// MaterialVk CLASS ----------------------------------------------------

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class MaterialVk : public Material {
public:
  MaterialVk() { PC_PRINT("CREATED", TagType::Constr, "MaterialVk.h") };
  virtual ~MaterialVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "MaterialVk.h")
  };

private:
  void MakeDescriptors();
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
