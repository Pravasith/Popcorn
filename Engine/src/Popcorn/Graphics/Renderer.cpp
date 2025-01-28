#include "Renderer.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Events/WindowEvent.h"
#include "RendererOpenGL.h"
#include "RendererVk.h"
#include "VertexBuffer.h"
#include <glm/glm.hpp>
#include <string>

ENGINE_NAMESPACE_BEGIN
GFX_NAMESPACE_BEGIN
// SINGLETON
Renderer *Renderer::s_instance = nullptr;
RendererType Renderer::s_type = RendererType::Vulkan;
std::variant<RendererVk *, RendererOpenGL *> Renderer::s_renderer{
    static_cast<RendererVk *>(nullptr)};

Renderer::Renderer(const Window &appWin) : m_AppWin(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER");
};

Renderer::~Renderer() { PC_PRINT("DESTROYED", TagType::Destr, "RENDERER") };

void Renderer::Init() const {
  if (s_type == RendererType::Vulkan) {
    s_renderer = new RendererVk(m_AppWin);

    // TODO: SET VERTEX BUFFER HERE
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

    // CREATES V-Bfr-Vk*
    // V-Bfr-Vk has m_vertex
    VertexBuffer *bfr = VertexBuffer::Create();
    bfr->Fill<Vertex>({
        {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    });

    auto layout = VertexBuffer::Layout();
    layout.Set<ElementTypes::Float2, ElementTypes::Float3>();

    for (auto elType : layout.elementTypes) {
      PC_PRINT(static_cast<int>(elType), TagType::Print, "ELEMENT TYPES")
    }

    auto *vkRenderer = std::get<RendererVk *>(s_renderer);
    vkRenderer
        // bfr's resources are moved to RendererVK's m_vertexBufferVk's;
        ->BindVertexBuffer(*(static_cast<VertexBufferVk *>(bfr)));

    VertexBuffer::Destroy(bfr);
    vkRenderer->InitVulkan();
    PC_PRINT("PRINT BUFFER::: " << &bfr, TagType::Print, "RENDERER")
    // bfr->PrintBuffer<Vertex>();

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
