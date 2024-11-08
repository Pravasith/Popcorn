#include "Renderer.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "RendererOpenGL.h"
#include "RendererVulkan.h"
#include <string>

// #include "Application.h"

ENGINE_NAMESPACE_BEGIN
// SINGLETON
Renderer *Renderer::s_instance = nullptr;
RendererType Renderer::s_type = RendererType::OpenGL;
const void *Renderer::s_osWindow = nullptr;
std::variant<RendererVulkan *, RendererOpenGL *> Renderer::s_renderer{
    static_cast<RendererVulkan *>(nullptr)};

Renderer::Renderer() {
  PC_PRINT_DEBUG("RENDERER CREATED", 1, "RENDERER");

  // TODO: CHANGE TO FANCY DISPATCHER STUFF
  s_type = RendererType::Vulkan;
};

Renderer::~Renderer() { PC_PRINT_DEBUG("RENDERER DESTROYED", 1, "RENDERER") };

void Renderer::Create() {
  if (s_instance) {
    return;
  }

  s_instance = new Renderer();
};

void Renderer::Run() {
  PC_PRINT_DEBUG(s_osWindow, 2, "RENDERER")

  // TODO: CHANGE TO FANCY DISPATCHER STUFF
  if (static_cast<int>(s_type) & static_cast<int>(RendererType::OpenGL)) {
    s_renderer = new RendererOpenGL();
  } else {
    s_renderer = new RendererVulkan();
  };
};

void Renderer::SetOSWindow(const void *osWindow) { s_osWindow = osWindow; };

Renderer &Renderer::Get() {
  PC_ASSERT(s_instance, "NO RENDERER INSTANCE");
  return *s_instance;
};

void Renderer::Destroy() {
  if (auto vulkanRenderer =
          std::get_if<RendererVulkan *>(&Renderer::s_renderer)) {
    delete *vulkanRenderer;
  } else if (auto openGLRenderer =
                 std::get_if<RendererOpenGL *>(&Renderer::s_renderer)) {
    delete *openGLRenderer;
  };

  Renderer::s_renderer = static_cast<RendererVulkan *>(nullptr);
  delete s_instance;
  s_instance = nullptr;
};

ENGINE_NAMESPACE_END
