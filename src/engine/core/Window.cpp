#include "Window.h"
#include <GLFW/glfw3.h>
#include <iostream>

Window *Window::instance = nullptr;
const char *Window::__dir = nullptr;
GLFWwindow *Window::__glfw_window = nullptr;

void Window::Create(const char *dir) {
  if (!instance) {
    instance = new Window();
    __dir = dir;
  }
}

Window *Window::Get() { return instance; }

void Window::Destroy() {
  if (instance) {
    delete instance;
    instance = nullptr;
  }
}

Window::Window() {
  InitGLFW();
  CreateGLFWWindow();
  std::cout << "WINDOW CREATED" << "\n";
};

Window::~Window() { std::cout << "WINDOW DESTROYED" << "\n"; };

void Window::InitGLFW() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::CreateGLFWWindow() {
  __glfw_window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

  if (!__glfw_window) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(__glfw_window);
}
