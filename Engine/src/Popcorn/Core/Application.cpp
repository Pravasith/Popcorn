#include "Application.h"
#include "Base.h"
#include "DebugUIOverlay.h"
#include "Renderer.h"

ENGINE_NAMESPACE_BEGIN
Application *Application::s_instance = nullptr;
LayerStack *Application::s_layerStack = nullptr;
Window *Application::s_window = nullptr;
Time *Application::s_time = nullptr;
Renderer *Application::s_Renderer = nullptr;
DebugUIOverlay *Application::s_debugUIOverlay = nullptr;

Application::Application() : Subscriber("Application") {
  PC_PRINT("APPLICATION STARTED", TagType::Constr, "APP");
};

Application::~Application() {
  Time::Destroy();
  // LAYERS ARE DELETED INTERNALLY IN THE LAYERSTACK DESTRUCTOR
  delete s_layerStack;
  s_layerStack = nullptr;

  Renderer::Destroy();
  s_Renderer = nullptr;

  // s_time->UnSubscribe(s_instance);
  // s_window->UnSubscribe(s_instance);

  Popcorn::Window::Destroy();

  PC_PRINT("APPLICATION STOPPED", TagType::Destr, "APP");
};

Application &Application::Get() { return *s_instance; }
Window &Application::GetAppWindow() const { return *s_window; }

void Application::Start() {
  if (!s_instance) {
    // DONT MOVE THIS BLOCK
    auto windowProps = Window::Props("Triangle App", 500, 500);
    auto AppWin = Window::Create(windowProps);

    s_window = AppWin;
    s_instance = new Application();

    s_window->Subscribe(s_instance);

    s_layerStack = new LayerStack();
    s_time = Time::Create();
    s_time->Subscribe(s_instance);

    // OVERLAYS - EXAMPLE
    // s_debugUIOverlay = new DebugUIOverlay();
    // s_layerStack->PushOverlay(s_debugUIOverlay);
    // s_debugUIOverlay->OnAttach();

    // LAYERS - EXAMPLE
    // auto s_renderLayer = new RenderLayer();
    // s_layerStack->PushLayer(s_renderLayer);
    // s_renderLayer->OnAttach();

  } else {
    PC_WARN(
        "ATTEMPT TO CREATE APPLICATION CLASS, WHEN INSTANCE ALREADY EXISTS");
  }
}

void Application::StartGameLoop() { s_time->Start(); };

bool Application::IsGameLoopRunning() { return s_time->IsGameLoopRunning(); };

void Application::AddLayer(Layer *layer) {
  s_layerStack->PushLayer(layer);
  layer->OnAttach();
};

void Application::Stop() {
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

bool Application::OnWindowClose(WindowCloseEvent &e) const {
  s_time->Stop();
  return true;
};

bool Application::OnUpdate(TimeEvent &e) {
  // Window::OnUpdate(e);
  // // RENDER LAYER UPDATES HERE
  // s_layerStack->UpdateLayerStack();
  // // s_Renderer->DrawFrame();
  return true;
};

bool Application::OnClockTick(TimeEvent &e) {
  Window::OnUpdate(e);
  // RENDER LAYER UPDATES HERE
  s_layerStack->UpdateLayerStack();
  // s_Renderer->DrawFrame();
  return true;
};

void Application::OnEvent(Event &e) {
  EventDispatcher dispatcher{e};

  dispatcher.Dispatch<WindowResizeEvent>(
      PC_BIND_EVENT_FUNC(WindowResizeEvent, OnWindowResize));

  if (s_Renderer) {
    dispatcher.Dispatch<FrameBfrResizeEvent>(
        PC_BIND_EVENT_FUNC(FrameBfrResizeEvent, s_Renderer->OnFrameBfrResize));
  }

  dispatcher.Dispatch<WindowCloseEvent>(
      PC_BIND_EVENT_FUNC(WindowCloseEvent, OnWindowClose));

  dispatcher.Dispatch<TimeEvent>(PC_BIND_EVENT_FUNC(TimeEvent, OnClockTick));

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
