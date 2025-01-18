#include "Application.h"
#include "Base.h"
#include "Renderer.h"
#include <stdexcept>

ENGINE_NAMESPACE_BEGIN
Application *Application::s_instance = nullptr;
LayerStack *Application::s_layerStack = nullptr;
Window *Application::s_window = nullptr;
Time *Application::s_time = nullptr;
Renderer *Application::s_Renderer = nullptr;

Application::Application() {
  PC_PRINT("APPLICATION STARTED", TagType::Constr, "APP");
};

Application::~Application() {

  // LAYERS ARE DELETED INTERNALLY IN THE LAYERSTACK DESTRUCTOR
  delete s_layerStack;

  Renderer::Destroy();
  s_Renderer = nullptr;

  Window::UnSubscribe(s_instance);
  Window::Destroy();

  PC_PRINT("APPLICATION STOPPED", TagType::Destr, "APP");
};

Application &Application::Get() { return *s_instance; }
Window &Application::GetAppWindow() const { return *s_window; }

void Application::Init() {
  if (!s_instance) {
    // DONT MOVE THIS BLOCK
    auto windowProps = Popcorn::Window::Props("Triangle App", 500, 500);
    auto AppWin = Popcorn::Window::Create(windowProps);

    s_window = AppWin;
    s_instance = new Application();

    Window::Subscribe(s_instance);

    s_layerStack = new LayerStack();

    // OVERLAYS - EXAMPLE
    // s_debugUIOverlay = new DebugUIOverlay();
    // s_layerStack->PushOverlay(s_debugUIOverlay);
    // s_debugUIOverlay->OnAttach();

    // LAYERS - EXAMPLE
    // auto s_renderLayer = new RenderLayer();
    // s_layerStack->PushLayer(s_renderLayer);
    // s_renderLayer->OnAttach();

    s_Renderer = Renderer::Create<RendererType::Vulkan>(*AppWin);

    s_time = Time::Get();
  } else {
    std::runtime_error(
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

bool Application::OnWindowClose(WindowCloseEvent &e) const {
  s_time->Stop();
  return true;
};

bool Application::OnCPUClockTick(TimeEvent &e) const {
  Window::OnUpdate();
  // RENDER LAYER UPDATES HERE
  s_layerStack->UpdateLayerStack();
  s_Renderer->DrawFrame();
  return true;
};

void Application::OnEvent(Event &e) const {
  EventDispatcher dispatcher{e};

  dispatcher.Dispatch<WindowResizeEvent>(
      PC_BIND_EVENT_FUNC(WindowResizeEvent, OnWindowResize));

  dispatcher.Dispatch<FrameBfrResizeEvent>(
      PC_BIND_EVENT_FUNC(FrameBfrResizeEvent, s_Renderer->OnFrameBfrResize));

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
