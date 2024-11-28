#include "RenderLayer.h"
#include "Application.h"
#include "Base.h"
#include "Global_Macros.h"
#include "Renderer.h"

ENGINE_NAMESPACE_BEGIN
RenderLayer::RenderLayer() {
  PC_PRINT("CREATED", TagType::Constr, "RENDER-LAYER");
  m_Renderer = Renderer::Create();
}

RenderLayer::~RenderLayer() {
  PC_PRINT("DESTROYED", TagType::Destr, "RENDER-LAYER");
}

void RenderLayer::OnAttach() {
  Application &app = Application::Get();
  auto osWindow = app.GetAppWindow().GetOSWindow();

  Renderer::SetOSWindow(osWindow);
  m_Renderer->Init();
}

void RenderLayer::OnDetach() { Renderer::Destroy(); }

void RenderLayer::OnUpdate() { m_Renderer->OnUpdate(); }

void RenderLayer::OnEvent(Event &e) {
  // TODO: ADD EVENTS
  std::cout << e.GetEventTypeName() << '\n';
}
ENGINE_NAMESPACE_END
