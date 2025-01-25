#include "RendererOpenGL.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
RendererOpenGL::RendererOpenGL(const Window &appWin)
    : Renderer(appWin) {
        PC_PRINT("CREATED", TagType::Constr, "RENDERER-OPENGL")
      };

RendererOpenGL::~RendererOpenGL() {
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER-OPENGL")
};
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
