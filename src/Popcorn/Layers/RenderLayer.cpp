#include "RenderLayer.h"
#include "Application.h"
#include "Base.h"
#include "Global_Macros.h"
#include "Renderer.h"

ENGINE_NAMESPACE_BEGIN
template <RendererType T> Renderer<T> *RenderLayer::s_Renderer = nullptr;

RenderLayer::RenderLayer() {
  PC_PRINT("CREATED", TagType::Constr, "RENDER-LAYER");
}

RenderLayer::~RenderLayer() {
  PC_PRINT("DESTROYED", TagType::Destr, "RENDER-LAYER");
}

void RenderLayer::OnAttach() {
  Application &app = Application::Get();
  auto &appWin = app.GetAppWindow();

  s_Renderer<RendererType::Vulkan> =
      Renderer<RendererType::Vulkan>::Create(appWin);
}

void RenderLayer::OnUpdate() { s_Renderer<RendererType::Vulkan>->OnUpdate(); }

void RenderLayer::OnEvent(Event &e) {
  s_Renderer<RendererType::Vulkan>->OnEvent(e);
}

void RenderLayer::OnDetach() { Renderer<RendererType::Vulkan>::Destroy(); }
ENGINE_NAMESPACE_END
