#include "Renderer.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "RendererOpenGL.h"
#include "RendererVk.h"
#include <string>

ENGINE_NAMESPACE_BEGIN
// SINGLETON
Renderer *Renderer::s_instance = nullptr;
RendererType Renderer::s_type = RendererType::OpenGL;
const void *Renderer::s_osWindow = nullptr;
std::variant<RendererVk *, RendererOpenGL *> Renderer::s_renderer{
    static_cast<RendererVk *>(nullptr)};

Renderer::Renderer() {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER");

  // TODO: CHANGE TO FANCY DISPATCHER STUFF
  s_type = RendererType::Vulkan;
};

Renderer::~Renderer() { PC_PRINT("DESTROYED", TagType::Destr, "RENDERER") };

void Renderer::Create() {
  if (s_instance) {
    return;
  }

  s_instance = new Renderer();
};

void Renderer::Run() {

  // TODO: CHANGE TO FANCY DISPATCHER STUFF
  if (static_cast<int>(s_type) & static_cast<int>(RendererType::OpenGL)) {
    s_renderer = new RendererOpenGL();
  } else {
    s_renderer = new RendererVk();
  };
};

void Renderer::SetOSWindow(const void *osWindow) { s_osWindow = osWindow; };

Renderer &Renderer::Get() {
  PC_ASSERT(s_instance, "NO RENDERER INSTANCE");
  return *s_instance;
};

void Renderer::Destroy() {
  if (auto vulkanRenderer = std::get_if<RendererVk *>(&Renderer::s_renderer)) {
    delete *vulkanRenderer;
  } else if (auto openGLRenderer =
                 std::get_if<RendererOpenGL *>(&Renderer::s_renderer)) {
    delete *openGLRenderer;
  };

  Renderer::s_renderer = static_cast<RendererVk *>(nullptr);

  delete s_instance;
  s_instance = nullptr;
};

ENGINE_NAMESPACE_END
