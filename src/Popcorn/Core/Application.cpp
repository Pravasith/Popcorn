#include "Application.h"
#include "Event.h"
#include "ImGuiLayer.h"
#include "LayerStack.h"
#include "Popcorn/Graphics/Renderer.h"
#include "Utilities.h"
#include "Window.h"
#include "WindowEvent.h"
#include <cassert>
#include <iostream>
#include <string>

ENGINE_NAMESPACE_BEGIN
Application *Application::s_instance = nullptr;
Window *Application::s_window = nullptr;
LayerStack *Application::s_layer_stack = nullptr;
ImGuiLayer *Application::s_imgui_layer = nullptr;
bool Application::s_is_game_loop_running = false;
Renderer *Application::s_renderer = nullptr;

Application::Application() {
  PC_PRINT_DEBUG("APPLICATION STARTED", 0, "APP");

  /* Window::Create(Window::Props("Ramen")); */
  /* Window::Destroy(); */
};

Application::~Application() {

  // DEALLOC MEMBERS
  delete s_layer_stack;

  // LAYERS ARE DELETED IN THE LAYERSTACK DESTRUCTOR
  Renderer::Destroy();

  Window::UnSubscribe(s_instance);
  Window::Destroy();

  PC_PRINT_DEBUG("APPLICATION STOPPED", 0, "APP");
};

Application &Application::Get() { return *s_instance; }
Window &Application::GetAppWindow() const { return *s_window; }

void Application::Start() {
  if (!s_instance) {
    // DONT MOVE THIS BLOCK
    // DONT MOVE THIS BLOCK
    s_instance = new Application();
    // DONT MOVE THIS BLOCK
    s_window = Window::Create(Window::Props("Popcorn Engine", 500, 500));
    Window::Subscribe(s_instance);
    // DONT MOVE THIS BLOCK
    Renderer::Create();
    // DONT MOVE THIS BLOCK
    Application::InitLayers();
    s_layer_stack = new LayerStack();
    // DONT MOVE THIS BLOCK
    s_imgui_layer = new ImGuiLayer();
    s_imgui_layer->OnAttach();
    // DONT MOVE THIS BLOCK
    s_layer_stack->PushLayer(s_imgui_layer);
    // DONT MOVE THIS BLOCK
    // DONT MOVE THIS BLOCK
  } else {
    write_log(
        "Attempt to create Application class, when instance already exists");
  }
}

void Application::InitLayers() {};

void Application::Run() {
  assert(!s_is_game_loop_running && "GAME LOOP ALREADY RUNNING!");

  s_is_game_loop_running = true;

  while (s_is_game_loop_running) {
    Window::OnUpdate();
    s_layer_stack->UpdateLayerStack();
  }
};

void Application::Stop() {

  // DESTROY WINDOW
  if (s_instance) {
    delete s_instance;
    s_instance = nullptr;
  }
}

bool Application::OnWindowResize(WindowResizeEvent &e) const { return true; };

bool Application::OnWindowClose(WindowCloseEvent &e) const {
  s_is_game_loop_running = false;
  return true;
};

void Application::OnEvent(Event &e) const {
  EventDispatcher dispatch(e);

  dispatch.Dispatch<WindowResizeEvent>(
      PC_BIND_EVENT_FUNC(WindowResizeEvent, OnWindowResize));

  dispatch.Dispatch<WindowCloseEvent>(
      PC_BIND_EVENT_FUNC(WindowCloseEvent, OnWindowClose));

  // auto layerStack = Application::GetLayerStack();
  // layerStack.IterateBackwards([&](Event &e) {
  //   // auto x = e.PrintDebugData();
  //   std::cout << e.PrintDebugData();
  // });

  if (e.BelongsToCategory(EventCategory::MouseEvent)) {
    std::cout << e.PrintDebugData();
  };

  if (e.BelongsToCategory(EventCategory::KeyboardEvent)) {
    std::cout << e.PrintDebugData();
  };
};

ENGINE_NAMESPACE_END
