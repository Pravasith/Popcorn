#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class Mesh {
public:
  Mesh() { PC_PRINT("CREATED", TagType::Constr, "MESH"); };
  ~Mesh() { PC_PRINT("DESTROYED", TagType::Destr, "MESH"); };
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
