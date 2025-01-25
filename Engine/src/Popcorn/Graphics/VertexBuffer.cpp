#include "VertexBuffer.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Renderer.h"
#include "VertexBufferVk.h"

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
VertexBuffer *VertexBuffer::Init() const {
  switch (Renderer::GetAPI()) {
  case RendererType::Vulkan: {
    return new VertexBufferVk();
  }
  case RendererType::OpenGL:
    // return new VertexBufferOpenGL();
    return nullptr;
  default:
    PC_ERROR("WRONG RENDERER API", "RENDERER-API")
    return nullptr;
  };
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
