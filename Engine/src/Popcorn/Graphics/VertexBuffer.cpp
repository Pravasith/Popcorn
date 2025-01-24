#include "VertexBuffer.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Renderer.h"
#include "VertexBufferVk.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
void VertexBuffer::Init() const {
  PC_PRINT("CREATED", TagType::Constr, "VERTEX-BUFFER")
  switch (Renderer::GetAPI()) {
  case RendererType::Vulkan: {
    PC_PRINT("VULKAN RENDERER", TagType::Print, "VERTEX-BUFFER")
    VertexBufferVk x;
    break;
  }
  case RendererType::OpenGL:
    PC_PRINT("OPENGL RENDERER", TagType::Print, "VERTEX-BUFFER")
    break;
  default:
    PC_ERROR("WRONG RENDERER API", "RENDERER-API")
    break;
  };
};
VertexBuffer::~VertexBuffer() {
  PC_PRINT("DESTROYED", TagType::Destr, "VERTEX-BUFFER")
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
