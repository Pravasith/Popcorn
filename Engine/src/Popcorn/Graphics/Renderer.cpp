#include "Renderer.h"
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

Renderer::~Renderer() { PC_PRINT("DESTROYED", TagType::Destr, "RENDERER") };

void Renderer::Init(const Window &appWin) {
  if (s_instance) {
    PC_WARN("Renderer instance (s_instance) exists!")
    return;
  };

  if (s_type == RendererType::None) {
    PC_WARN("Attempt to create renderer without setting it's type")
    return;
  };

  if (s_type == RendererType::Vulkan) {
    s_instance = new RendererVk(appWin);

    // // ----------------------------------------------------------
    // // Vertex buffer --------------------------------------------
    // // ----------------------------------------------------------
    // struct Vertex {
    //   glm::vec2 pos;
    //   glm::vec3 color;
    //   std::string Print() {
    //     std::stringstream ss;
    //     ss << pos.x << ", " << pos.y << "; " << color.r << ", " << color.g
    //        << ", " << color.b;
    //
    //     return ss.str();
    //   };
    // };
    //
    // s_vertexBuffer = VertexBuffer::Create();
    // s_vertexBuffer->Fill<Vertex>({
    //     {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
    //     {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    //     {{0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    // });
    //
    // s_vertexBuffer->SetLayout<VertexBuffer::AttrTypes::Float2,
    //                           VertexBuffer::AttrTypes::Float3>();
    //
    // // vkRenderer->BindVertexBuffer(static_cast<VertexBufferVk
    // *>(s_vertexBuffer));
    // s_vertexBuffer->PrintBuffer<Vertex>();
    // ----------------------------------------------------------
    // Vertex buffer --------------------------------------------
    // ----------------------------------------------------------

    RendererVk *vkRenderer = static_cast<RendererVk *>(s_instance);
    vkRenderer->VulkanInit();
    vkRenderer->CreateRenderWorkflows();
    vkRenderer->CreateBasicCommandBuffers();
    vkRenderer->AllocateVkBuffers();

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
