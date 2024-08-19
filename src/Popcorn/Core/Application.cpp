#include "Application.h"
#include "Event.h"
#include "ImGuiLayer.h"
#include "LayerStack.h"
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

Application::Application() {
  PC_PRINT_DEBUG("ENGINE STARTED", 0, "APP");

  /* Window::Create(Window::Props("Ramen")); */
  /* Window::Destroy(); */
};

Application::~Application() {

  // DELETE LAYER STACK
  delete s_layer_stack;

  // DESTROY WINDOW
  Window::Destroy();

  PC_PRINT_DEBUG("ENGINE STOPPED", 0, "APP");
};

Application &Application::Get() { return *s_instance; }
Window &Application::GetAppWindow() const { return *s_window; }

void Application::Start() {
  PC_PRINT_DEBUG("APPLICATION STARTED", 0, "APP");

  if (!s_instance) {
    s_instance = new Application();
    s_window = Window::Create(Window::Props("Popcorn Engine", 500, 500));

    Window::AddSubscriber(s_instance);

    s_layer_stack = new LayerStack();

    s_imgui_layer = new ImGuiLayer();
    s_imgui_layer->OnAttach();

    s_layer_stack->PushLayer(s_imgui_layer);
  } else {
    write_log(
        "Attempt to create Application class, when instance already exists");
  }
}

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
