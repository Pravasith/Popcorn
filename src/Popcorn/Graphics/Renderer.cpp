#include "Renderer.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Assert.h"
#include "Popcorn/Core/Base.h"
#include "RendererOpenGL.h"
#include "RendererVk.h"
#include <string>

ENGINE_NAMESPACE_BEGIN
// SINGLETON
template <RendererType T> Renderer<T> *Renderer<T>::s_instance = nullptr;
template <RendererType T>
std::variant<RendererVk *, RendererOpenGL *> Renderer<T>::s_renderer{
    static_cast<RendererVk *>(nullptr)};

template <RendererType T>
Renderer<T>::Renderer(const Window &appWin) : m_AppWin(appWin) {
  PC_PRINT("CREATED", TagType::Constr, "RENDERER");
};

template <RendererType T>
Renderer<T>::~Renderer(){PC_PRINT("DESTROYED", TagType::Destr, "RENDERER")};

template <RendererType T>
Renderer<T> *Renderer<T>::Create(const Window &appWin) {
  if (!s_instance) {
    // PC_ASSERT(s_instance, "NO RENDERER INSTANCE");
    s_instance = new Renderer(appWin);
    s_instance->Init();
  }

  return s_instance;
};

template <RendererType T> void Renderer<T>::Init() const {
  // TODO: CHANGE TO FANCY DISPATCHER STUFF
  if constexpr (static_cast<int>(s_type) &
                static_cast<int>(RendererType::OpenGL)) {
    s_renderer = new RendererOpenGL();
  } else {
    s_renderer = new RendererVk(m_AppWin);
  };
};

template <RendererType T> const auto Renderer<T>::GetRenderer() {
  // PC_ASSERT(s_renderer, "NO RENDERER INSTANCE");
  if constexpr (T == RendererType::Vulkan) {
    return std::get<RendererVk *>(s_renderer);
  } else if constexpr (T == RendererType::OpenGL) {
    return std::get<RendererOpenGL *>(s_renderer);
  } else {
    PC_STATIC_ASSERT(true, "UNSUPPORTED RENDERERTYPE");
  }
};

template <RendererType T> void Renderer<T>::OnUpdate() {
  if constexpr (T == RendererType::Vulkan) {
    std::get<RendererVk *>(s_renderer)->OnUpdate();
  } else {
    // std::get<RendererOpenGL *>(s_renderer)->OnUpdate();
  }
}

template <RendererType T> void Renderer<T>::Destroy() {
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
template class Renderer<RendererType::OpenGL>;
template class Renderer<RendererType::Vulkan>;

ENGINE_NAMESPACE_END
