#include "Application.h"
#include "Base.h"
// #include "DebugUIOverlay.h"
#include "RenderLayer.h"
#include <stdexcept>

ENGINE_NAMESPACE_BEGIN
Application *Application::s_instance = nullptr;
LayerStack *Application::s_layerStack = nullptr;
Window *Application::s_window = nullptr;
Time *Application::s_time = nullptr;

Application::Application() {
  PC_PRINT("APPLICATION STARTED", TagType::Constr, "APP");

  /* Window::Create(Window::Props("Ramen")); */
  /* Window::Destroy(); */
};

Application::~Application() {

  // DEALLOC MEMBERS
  delete s_layerStack;

  // LAYERS ARE DELETED IN THE LAYERSTACK DESTRUCTOR

  Window::UnSubscribe(s_instance);
  Window::Destroy();

  PC_PRINT("APPLICATION STOPPED", TagType::Destr, "APP");
};

Application &Application::Get() { return *s_instance; }
Window &Application::GetAppWindow() const { return *s_window; }

void Application::Start(Window *appWin) {
  if (!s_instance) {
    // DONT MOVE THIS BLOCK
    s_window = appWin;
    s_instance = new Application();
    Window::Subscribe(s_instance);

    Application::InitLayers();
    s_layerStack = new LayerStack();

    // OVERLAYS
    // s_debugUIOverlay = new DebugUIOverlay();
    // s_layerStack->PushOverlay(s_debugUIOverlay);
    // s_debugUIOverlay->OnAttach();

    // LAYERS
    static auto s_renderLayer = new RenderLayer();
    s_layerStack->PushLayer(s_renderLayer);
    s_renderLayer->OnAttach();

    s_time = Time::Get();
  } else {
    std::runtime_error(
        "ATTEMPT TO CREATE APPLICATION CLASS, WHEN INSTANCE ALREADY EXISTS");
  }
}

void Application::InitLayers() {};

void Application::Run() { s_time->Start(); };

bool Application::IsGameLoopRunning() { return s_time->IsGameLoopRunning(); };

void Application::Stop() {
  // DESTROY WINDOW
  if (s_instance) {
    delete s_instance;
    s_instance = nullptr;
  }
}

bool Application::OnWindowResize(WindowResizeEvent &e) const {
  // s_layerStack->IterateBackwards([&](auto it) {
  //   PC_PRINT(&it, TagType::Print, "ONRESIZE")
  //
  //   if (e.IsHandled()) {
  //     return;
  //   }
  //
  //   (*it)->OnEvent(e);
  // });

  return true;
};

bool Application::OnFrameBfrResize(FrameBfrResizeEvent &e) const {
  s_layerStack->IterateBackwards([&](auto it) {
    if (e.IsHandled()) {
      return;
    }

    (*it)->OnEvent(e);
  });

  return true;
};

bool Application::OnWindowClose(WindowCloseEvent &e) const {
  s_time->Stop();
  return true;
};

bool Application::OnCPUClockTick(TimeEvent &e) const {
  Window::OnUpdate();
  s_layerStack->UpdateLayerStack();
  return true;
};

void Application::OnEvent(Event &e) const {
  EventDispatcher dispatcher{e};

  dispatcher.Dispatch<WindowResizeEvent>(
      PC_BIND_EVENT_FUNC(WindowResizeEvent, OnWindowResize));

  dispatcher.Dispatch<FrameBfrResizeEvent>(
      PC_BIND_EVENT_FUNC(FrameBfrResizeEvent, OnFrameBfrResize));

  dispatcher.Dispatch<WindowCloseEvent>(
      PC_BIND_EVENT_FUNC(WindowCloseEvent, OnWindowClose));

  dispatcher.Dispatch<TimeEvent>(PC_BIND_EVENT_FUNC(TimeEvent, OnCPUClockTick));

  // auto layerStack = Application::GetLayerStack();
  // layerStack.IterateBackwards([&](Event &e) {
  //   // auto x = e.PrintDebugData();
  //   std::cout << e.PrintDebugData();
  // });

  if (e.BelongsToCategory(EventCategory::MouseEvent)) {
    // e.PrintDebugData();
  };

  if (e.BelongsToCategory(EventCategory::KeyboardEvent)) {
    // e.PrintDebugData();
  };
};

ENGINE_NAMESPACE_END
