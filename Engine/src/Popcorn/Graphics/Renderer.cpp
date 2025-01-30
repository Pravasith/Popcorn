#include "Renderer.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Events/WindowEvent.h"
#include "RendererOpenGL.h"
#include "RendererVk.h"
#include "VertexBuffer.h"
#include "VertexBufferVk.h"
#include <glm/glm.hpp>
#include <string>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
// SINGLETON
Renderer *Renderer::s_instance = nullptr;
RendererType Renderer::s_type = RendererType::Vulkan;
std::variant<RendererVk *, RendererOpenGL *> Renderer::s_renderer{
    static_cast<RendererVk *>(nullptr)};

VertexBuffer *Renderer::s_vertexBuffer = nullptr;

Renderer::Renderer(const Window &appWin) : m_AppWin(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER");
};

Renderer::~Renderer() { PC_PRINT("DESTROYED", TagType::Destr, "RENDERER") };

void Renderer::Init() const {
  if (s_type == RendererType::Vulkan) {
    s_renderer = new RendererVk(m_AppWin);

    // ----------------------------------------------------------
    // Vertex buffer --------------------------------------------
    // ----------------------------------------------------------
    struct Vertex {
      glm::vec2 pos;
      glm::vec3 color;
      std::string Print() {
        std::stringstream ss;
        ss << pos.x << ", " << pos.y << "; " << color.r << ", " << color.g
           << ", " << color.b;

        return ss.str();
      };
    };

    s_vertexBuffer = VertexBuffer::Create();
    s_vertexBuffer->Fill<Vertex>({
        {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    });

    s_vertexBuffer->SetLayout<VertexBuffer::AttrTypes::Float2,
                              VertexBuffer::AttrTypes::Float3>();

    auto *vkRenderer = std::get<RendererVk *>(s_renderer);
    vkRenderer->BindVertexBuffer(static_cast<VertexBufferVk *>(s_vertexBuffer));
    vkRenderer->InitVulkan();

    // s_vertexBuffer->PrintBuffer<Vertex>();
    // ----------------------------------------------------------
    // Vertex buffer --------------------------------------------
    // ----------------------------------------------------------

  } else if (s_type == RendererType::OpenGL) {
    s_renderer = new RendererOpenGL(m_AppWin);
  } else {
    PC_STATIC_ASSERT(true, "UNSUPPORTED RENDERER TYPE");
  }
};

Renderer *Renderer::GetRenderer() {
  // PC_ASSERT(s_renderer, "NO RENDERER INSTANCE");
  if (s_type == RendererType::Vulkan) {
    return std::get<RendererVk *>(s_renderer);
  } else if (s_type == RendererType::OpenGL) {
    return std::get<RendererOpenGL *>(s_renderer);
  } else {
    PC_STATIC_ASSERT(true, "UNSUPPORTED RENDERER TYPE");
    return nullptr;
  }
};

void Renderer::DrawFrame() {
  if (s_type == RendererType::Vulkan) {
    // Use GDB
    std::get<RendererVk *>(s_renderer)->DrawFrame();
  } else {
    // std::get<RendererOpenGL *>(s_renderer)->OnUpdate();
  }
}

bool Renderer::OnFrameBfrResize(FrameBfrResizeEvent &e) {
  if (s_type == RendererType::Vulkan) {
    return std::get<RendererVk *>(s_renderer)->OnFrameBfrResize(e);
  } else {
    // std::get<RendererOpenGL *>(s_renderer)->OnEvent();
  }

  return true;
}

void Renderer::Destroy() {
  if (auto vulkanRenderer = std::get_if<RendererVk *>(&Renderer::s_renderer)) {
    VertexBuffer::Destroy(s_vertexBuffer);
    delete *vulkanRenderer;
  } else if (auto openGLRenderer =
                 std::get_if<RendererOpenGL *>(&Renderer::s_renderer)) {
    delete *openGLRenderer;
  };

  Renderer::s_renderer = static_cast<RendererVk *>(nullptr);
  delete s_instance;
  s_instance = nullptr;
};

// Explicit template instantiation
// template class Renderer<RendererType::OpenGL>;
// template class Renderer<RendererType::Vulkan>;
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
