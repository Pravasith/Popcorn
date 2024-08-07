#include "Application.h"
#include "Event.h"
#include "Utilities.h"
#include "Window.h"
#include "WindowEvent.h"
#include <cassert>
#include <iostream>
#include <string>

ENGINE_NAMESPACE_BEGIN
Application *Application::s_instance = nullptr;
bool Application::s_is_game_loop_running = false;

Application::Application() {
  PC_PRINT_DEBUG("ENGINE STARTED", 0, "APP");

  /* Window::Create(Window::Props("Ramen")); */
  /* Window::Destroy(); */
};

Application::~Application() { PC_PRINT_DEBUG("ENGINE STOPPED", 0, "APP"); };

Application *Application::Get() { return s_instance; }

void Application::Start() {
  if (!s_instance) {
    s_instance = new Application();

    Window::Create(Window::Props("Popcorn Engine", 500, 500));
    Window::AddSubscriber(s_instance);

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
  }
};

void Application::Stop() {
  Window::Destroy();
  if (s_instance) {
    delete s_instance;
    s_instance = nullptr;
  }
}

bool Application::OnWindowResize(WindowResizeEvent &e) const {
  std::cout << "FROM NEW: " << e.PrintDebugData();
  return true;
};

bool Application::OnWindowClose(WindowCloseEvent &e) const {
  std::cout << "FROM NEW: " << e.PrintDebugData();

  s_is_game_loop_running = false;
  return true;
};

void Application::OnEvent(Event &e) const {
  EventDispatcher dispatch(e);

  dispatch.Dispatch<WindowResizeEvent>(
      PC_BIND_EVENT_FUNC(WindowResizeEvent, OnWindowResize));

  dispatch.Dispatch<WindowCloseEvent>(
      PC_BIND_EVENT_FUNC(WindowCloseEvent, OnWindowClose));

  if (e.BelongsToCategory(EventCategory::MouseEvent)) {
    std::cout << e.PrintDebugData();
  };

  if (e.BelongsToCategory(EventCategory::KeyboardEvent)) {
    std::cout << e.PrintDebugData();
  };
};

ENGINE_NAMESPACE_END
