#include "Application.h"
#include "Utilities.h"
#include "Window.h"
#include <iostream>

ENGINE_NAMESPACE_BEGIN
Application *Application::s_instance = nullptr;

Application *Application::Get() { return s_instance; }

void Application::Start() {
  if (!s_instance) {
    s_instance = new Application();
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

  Window::Create("Popcorn Application");
  m_window = Window::Get();

  if (!m_window) {
    write_log("Failed to create Window Instance in Application");
  }
};

Application::~Application() {
  Window::Destroy();
  m_window = nullptr;

  std::cout << "ENGINE STOPPED" << "\n";
};
ENGINE_NAMESPACE_END
