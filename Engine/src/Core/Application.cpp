#include "Popcorn/Core/Application.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace Popcorn {
Application *Application::s_instance = nullptr;

Application *Application::Get() {
  if (s_instance) {
    return s_instance;
  }
  return new Application();
}
void Application::Destroy() {
  if (s_instance) {
    delete s_instance;
    return;
  }
  std::cout << "Warning: Singleton already destroyed" << '\n';
}

Application::Application() {
  GLFWwindow *win =
      glfwCreateWindow(200, 200, "Popcorn Game", nullptr, nullptr);
  std::cout << "APPLICATION CREATED" << '\n';
}

Application::~Application() {}
} // namespace Popcorn
