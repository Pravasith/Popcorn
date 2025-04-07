#include "GfxContext.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

GfxContext *GfxContext::s_instance = nullptr;
ShaderLibrary *GfxContext::s_shaderLibrary = nullptr;

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
