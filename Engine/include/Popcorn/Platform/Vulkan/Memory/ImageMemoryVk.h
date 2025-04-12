
#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

//
// GRAPHICS CLASS ----------------------------------------------------

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

class ImageMemoryVk {
public:
  ImageMemoryVk() { PC_PRINT("CREATED", TagType::Constr, "ImageMemoryVk.h") };
  ~ImageMemoryVk() { PC_PRINT("DESTROYED", TagType::Destr, "ImageMemoryVk.h") };

private:
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
