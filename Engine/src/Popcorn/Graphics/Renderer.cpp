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

VertexBuffer *Renderer::s_vertexBuffer = nullptr;

Renderer::Renderer(const Window &appWin) : m_AppWin(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER");
};

Renderer::~Renderer() {
  ((RendererVk *)s_instance)->VulkanDestroy();
  PC_PRINT("DESTROYED", TagType::Destr, "RENDERER")
};

void Renderer::Init(const Window &appWin) {
  if (s_instance) {
    PC_WARN("Renderer instance (s_instance) exists!")
    return;
  };

  if (s_type == RendererType::Vulkan) {
    s_instance = new RendererVk(appWin);

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

    RendererVk *vkRenderer = static_cast<RendererVk *>(s_instance);
    // vkRenderer->BindVertexBuffer(static_cast<VertexBufferVk
    // *>(s_vertexBuffer));
    vkRenderer->VulkanInit();

    // TEMP FUNCTIONS
    // CREATE ALL PIPELINES
    // CREATE ALL RENDERPASSES
    vkRenderer->CreateTrianglePipeline();
    vkRenderer->CreateTriangleRenderPass();
    //

    // s_vertexBuffer->PrintBuffer<Vertex>();
    // ----------------------------------------------------------
    // Vertex buffer --------------------------------------------
    // ----------------------------------------------------------

  } else if (s_type == RendererType::OpenGL) {
    s_instance = new RendererOpenGL(appWin);
  } else {
    PC_STATIC_ASSERT(true, "UNSUPPORTED RENDERER TYPE");
  }
};

void Renderer::DrawFrame() {
  if (s_type == RendererType::Vulkan) {
    static_cast<RendererVk *>(s_instance)->DrawFrame();
  } else {
    // std::get<RendererOpenGL *>(s_renderer)->OnUpdate();
  }
}

bool Renderer::OnFrameBfrResize(FrameBfrResizeEvent &e) {
  if (s_type == RendererType::Vulkan) {
    return static_cast<RendererVk *>(s_instance)->OnFrameBfrResize(e);
  } else {
    // std::get<RendererOpenGL *>(s_renderer)->OnEvent();
  }

  return true;
}

void Renderer::Destroy() {
  VertexBuffer::Destroy(s_vertexBuffer);
  delete s_instance;

  s_instance = nullptr;
};

// Explicit template instantiation
// template class Renderer<RendererType::OpenGL>;
// template class Renderer<RendererType::Vulkan>;
GFX_NAMESPACE_END
ENGINE_NAMESPACE_END
