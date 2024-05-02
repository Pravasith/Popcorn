#include "Application.h"
#include "Window.h"
#include <iostream>
#include <string>

ENGINE_NAMESPACE_BEGIN
Application *Application::s_instance = nullptr;

Application *Application::Get() { return s_instance; }

void Application::Start() {
  if (!s_instance) {
    static Application *s_instance = new Application();
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

  Window::Create(Window::Props(title));
};

Application::~Application() { std::cout << "ENGINE STOPPED" << "\n"; };
ENGINE_NAMESPACE_END
