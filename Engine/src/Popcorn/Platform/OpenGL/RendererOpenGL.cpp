#include "RendererOpenGL.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN

RendererOpenGL::RendererOpenGL() {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER-OPENGL")
};

RendererOpenGL::~RendererOpenGL() {
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-OPENGL")
};
ENGINE_NAMESPACE_END
