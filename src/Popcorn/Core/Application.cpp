#include "Application.h"
#include "Base.h"
#include "RenderLayer.h"
#include "Utilities.h"

ENGINE_NAMESPACE_BEGIN
Application *Application::s_instance = nullptr;
LayerStack *Application::s_layer_stack = nullptr;
Window *Application::s_window = nullptr;
Time *Application::s_time = nullptr;

DebugUIOverlay *Application::s_debug_ui_overlay = nullptr;
RenderLayer *Application::s_render_layer = nullptr;

Application::Application() {
  PC_PRINT("APPLICATION STARTED", TagType::Constr, "APP");

  /* Window::Create(Window::Props("Ramen")); */
  /* Window::Destroy(); */
};

Application::~Application() {

  // DEALLOC MEMBERS
  delete s_layer_stack;

  // LAYERS ARE DELETED IN THE LAYERSTACK DESTRUCTOR

  Window::UnSubscribe(s_instance);
  Window::Destroy();

  PC_PRINT("APPLICATION STOPPED", TagType::Destr, "APP");
};

Application &Application::Get() { return *s_instance; }
Window &Application::GetAppWindow() const { return *s_window; }

void Application::Start() {
  if (!s_instance) {
    // DONT MOVE THIS BLOCK
    s_instance = new Application();
    s_window = Window::Create(Window::Props("Popcorn Engine", 500, 500));
    Window::Subscribe(s_instance);

    Application::InitLayers();
    s_layer_stack = new LayerStack();

    // OVERLAYS
    // s_debug_ui_overlay = new DebugUIOverlay();
    // s_layer_stack->PushOverlay(s_debug_ui_overlay);
    // s_debug_ui_overlay->OnAttach();

    // LAYERS
    s_render_layer = new RenderLayer();
    s_layer_stack->PushLayer(s_render_layer);
    s_render_layer->OnAttach();

    s_time = Time::Get();
  } else {
    pc_write_log(
        "Attempt to create Application class, when instance already exists");
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

bool Application::OnWindowResize(WindowResizeEvent &e) const { return true; };

bool Application::OnWindowClose(WindowCloseEvent &e) const {
  s_time->Stop();
  return true;
};

bool Application::OnCPUClockTick(TimeEvent &e) const {
  Window::OnUpdate();
  s_layer_stack->UpdateLayerStack();

  return true;
};

void Application::OnEvent(Event &e) const {
  EventDispatcher dispatch(e);

  dispatch.Dispatch<WindowResizeEvent>(
      PC_BIND_EVENT_FUNC(WindowResizeEvent, OnWindowResize));

  dispatch.Dispatch<WindowCloseEvent>(
      PC_BIND_EVENT_FUNC(WindowCloseEvent, OnWindowClose));

  dispatch.Dispatch<TimeEvent>(PC_BIND_EVENT_FUNC(TimeEvent, OnCPUClockTick));

  // auto layerStack = Application::GetLayerStack();
  // layerStack.IterateBackwards([&](Event &e) {
  //   // auto x = e.PrintDebugData();
  //   std::cout << e.PrintDebugData();
  // });

  if (e.BelongsToCategory(EventCategory::MouseEvent)) {
    e.PrintDebugData();
  };

  if (e.BelongsToCategory(EventCategory::KeyboardEvent)) {
    e.PrintDebugData();
  };
};

ENGINE_NAMESPACE_END
