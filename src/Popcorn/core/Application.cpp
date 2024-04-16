#include "Application.h"
#include "Utilities.h"
#include <iostream>

Application *Application::instance = nullptr;
Window *Application::window = nullptr;

Application *Application::Get() { return instance; }

void Application::Start() {
  if (!instance) {
    instance = new Application();
  }
}

void Application::Stop() {
  if (instance) {
    delete instance;
    instance = nullptr;
  }
}

Application::Application() {
  std::cout << "ENGINE STARTED" << "\n";
  Window::Create("Popcorn Application");
  window = Window::Get();

  if (!window) {
    write_log("Failed to create Window instance in Application");
  }
};

Application::~Application() {
  Window::Destroy();
  std::cout << "ENGINE STOPPED" << "\n";
};
