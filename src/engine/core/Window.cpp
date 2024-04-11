#include "Window.h"
#include "GLAD_Funcs.h"
#include "GLFW_Funcs.h"
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
  Init();
  CreateWindow();

  InitGraphicsLib();
  SetGraphicsViewport();

  while (!glfwWindowShouldClose(__glfw_window)) {
    glfwSwapBuffers(__glfw_window);
    glfwPollEvents();
  }

  glfwTerminate();
  std::cout << "WINDOW CREATED" << "\n";
};

Window::~Window() { std::cout << "WINDOW DESTROYED" << "\n"; };

void Window::Init() {
  GLFW_Funcs::Init(GLAD_Funcs::Set_Viewport);
  /* OR ANY OTHER LIB FUNCTION CALL */
}

void Window::CreateWindow() {
  GLFW_Funcs::Create_Window(__glfw_window, __dir);
  /* OR ANY OTHER LIB FUNCTION CALL */
}

void Window::InitGraphicsLib() {
  GLAD_Funcs::Init(GLFW_Funcs::Get_Proc_Address);
  /* OR ANY OTHER LIB FUNCTION CALL */
};

void Window::SetGraphicsViewport() {
  GLAD_Funcs::Set_Viewport(0, 0, 800, 600);
  /* OR ANY OTHER LIB FUNCTION CALL */
};
