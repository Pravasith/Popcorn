#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

//
// GRAPHICS CLASS ----------------------------------------------------

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class BufferMemoryVk {
public:
  BufferMemoryVk() { PC_PRINT("CREATED", TagType::Constr, "BufferMemoryVk.h") };
  ~BufferMemoryVk() {
    PC_PRINT("DESTROYED", TagType::Destr, "BufferMemoryVk.h")
  };

private:
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
