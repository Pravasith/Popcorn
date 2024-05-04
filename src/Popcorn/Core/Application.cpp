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

  const std::string &title = "Popcorn Application";
  auto *x = Window::Create(Window::Props(title));
  std::cout << "Window ptr 1: " << x << "\n";
  x = Window::Create(Window::Props("Popcorn Application 2"));
  std::cout << "Window ptr 2: " << x << "\n";
};

Application::~Application() { std::cout << "ENGINE STOPPED" << "\n"; };
ENGINE_NAMESPACE_END
