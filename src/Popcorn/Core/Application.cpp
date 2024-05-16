#include "Application.h"
#include "Utilities.h"
#include "Window.h"
#include <iostream>
#include <string>

ENGINE_NAMESPACE_BEGIN
Application *Application::s_instance = nullptr;

Application *Application::Get() { return s_instance; }

void Application::Start() {
  if (!s_instance) {
    s_instance = new Application();
  } else {
    write_log(
        "Attempt to create Application class, when instance already exists");
  }
}

void Application::Stop() {
  if (s_instance) {
    delete s_instance;
    s_instance = nullptr;
  }
}

Application::Application() {
  std::cout << "ENGINE STARTED" << "\n";
  ;
  Window::Create(Window::Props("Popcorn Engine"));
  Window::Destroy();

  /* Window::Create(Window::Props("Sakura")); */
  /* Window::Destroy(); */
};

Application::~Application() { std::cout << "ENGINE STOPPED" << "\n"; };
ENGINE_NAMESPACE_END
