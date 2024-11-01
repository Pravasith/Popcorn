
// // #include "Application.h"
// #include "RenderLayer.h"
// #include "Global_Macros.h"
//
// ENGINE_NAMESPACE_BEGIN
// RenderLayer::RenderLayer() { PC_PRINT_DEBUG("CREATED", 1, "RENDER-LAYER"); }
// RenderLayer::~RenderLayer() { PC_PRINT_DEBUG("DESTROYED", 1, "RENDER-LAYER");
// }
//
// void RenderLayer::OnAttach() {
//   Application &app = Application::Get();
//   m_os_window = app.GetAppWindow().GetOSWindow();
// }
//
// void RenderLayer::OnDetach() {}
//
// void RenderLayer::OnUpdate() {}
//
// void RenderLayer::OnEvent(Event &e) {
//   // TODO: ADD EVENTS
//   std::cout << e.GetEventTypeName() << '\n';
// }
// ENGINE_NAMESPACE_END
