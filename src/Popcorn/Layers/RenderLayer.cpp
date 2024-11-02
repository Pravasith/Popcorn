#include "RenderLayer.h"
#include "Application.h"
#include "Base.h"
#include "Global_Macros.h"
#include "Renderer.h"

ENGINE_NAMESPACE_BEGIN
RenderLayer::RenderLayer() { PC_PRINT_DEBUG("CREATED", 1, "RENDER-LAYER"); }
RenderLayer::~RenderLayer() { PC_PRINT_DEBUG("DESTROYED", 1, "RENDER-LAYER"); }

void RenderLayer::OnAttach() {
  Application &app = Application::Get();
  auto osWindow = app.GetAppWindow().GetOSWindow();

  Renderer::SetOSWindow(osWindow);
  Renderer::Create();
  Renderer::Run();
}

void RenderLayer::OnDetach() { Renderer::Destroy(); }

void RenderLayer::OnUpdate() {
  // Renderer::OnUpdate();
}

void RenderLayer::OnEvent(Event &e) {
  // TODO: ADD EVENTS
  std::cout << e.GetEventTypeName() << '\n';
}
ENGINE_NAMESPACE_END
