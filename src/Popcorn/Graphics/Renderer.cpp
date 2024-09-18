#include "Renderer.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "Renderer_OpenGL.h"
#include "Renderer_Vulkan.h"

ENGINE_NAMESPACE_BEGIN
// SINGLETON
Renderer *Renderer::s_instance = nullptr;

Renderer::Renderer() {
  PC_PRINT_DEBUG("RENDERER CREATED", 1, "RENDERER");

  RendererOpenGL();
  RendererVulkan();
};
Renderer::~Renderer() { PC_PRINT_DEBUG("RENDERER DESTROYED", 1, "RENDERER") };

void Renderer::Create() {
  if (s_instance) {
    return;
  }

  s_instance = new Renderer();
};

Renderer &Renderer::Get() const {
  PC_ASSERT(!s_instance, "NO RENDERER INSTANCE");
  return *s_instance;
};

void Renderer::Destroy() {
  delete s_instance;
  s_instance = nullptr;
};

ENGINE_NAMESPACE_END
