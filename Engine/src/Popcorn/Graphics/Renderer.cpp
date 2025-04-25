#include "Renderer.h"
#include "GfxContext.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "RendererOpenGL.h"
#include "RendererVk.h"
#include <glm/glm.hpp>
#include <string>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
// SINGLETON
Renderer *Renderer::s_instance = nullptr;
RendererType Renderer::s_type = RendererType::None;

Renderer::Renderer(const Window &appWin) : m_AppWin(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER");
};

Renderer::~Renderer() {
  GfxContext::Destroy();
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER")
};

void Renderer::Init(const Window &appWin) {
  if (s_instance) {
    PC_WARN("Renderer instance (s_instance) exists!")
    return;
  };

  if (s_type == RendererType::None) {
    PC_WARN("Attempt to create renderer without setting it's type")
    return;
  };

  s_gfxContext = GfxContext::Get();

  if (s_type == RendererType::Vulkan) {
    s_instance = new RendererVk(appWin); // Creates Vulkan context
  } else if (s_type == RendererType::OpenGL) {
    s_instance = new RendererOpenGL(appWin);
  } else {
    PC_STATIC_ASSERT(true, "UNSUPPORTED RENDERER TYPE");
  }
};

void Renderer::Destroy() {
  // VertexBuffer::Destroy(s_vertexBuffer);
  delete s_instance;

  s_instance = nullptr;
};

GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
