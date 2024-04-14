#include "Application.h"
#include "Window.h"
#include <iostream>

Application *Application::instance = nullptr;

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
  Window::Destroy();
};
Application::~Application() { std::cout << "ENGINE STOPPED" << "\n"; };
