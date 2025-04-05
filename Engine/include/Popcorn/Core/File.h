#pragma once

#include "Base.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN

class File {
public:
  File() { PC_PRINT("CREATED", TagType::Constr, "File"); }
  ~File() { PC_PRINT("DESTROYED", TagType::Destr, "File"); }
};

ENGINE_NAMESPACE_END
