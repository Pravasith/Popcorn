#include "ContextGfx.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN

ContextGfx *ContextGfx::s_instance = nullptr;

void ContextGfx::GraphicsInit() {}

void ContextGfx::GraphicsCleanUp() {}

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
