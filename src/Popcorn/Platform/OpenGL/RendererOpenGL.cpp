#include "RendererOpenGL.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN

RendererOpenGL::RendererOpenGL() {
  PC_PRINT_DEBUG("OPENGL CREATED", 2, "RENDERER")
};

RendererOpenGL::~RendererOpenGL() {
  PC_PRINT_DEBUG("OPENGL DESTROYED", 2, "RENDERER")
};
ENGINE_NAMESPACE_END
