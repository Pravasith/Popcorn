#pragma once

#include "Base.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class EXAMPLE {
public:
  EXAMPLE() { PC_PRINT("CREATED", TagType::Constr, "EXAMPLE.h") };
  ~EXAMPLE() { PC_PRINT("DESTROYED", TagType::Destr, "EXAMPLE.h") };

private:
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
