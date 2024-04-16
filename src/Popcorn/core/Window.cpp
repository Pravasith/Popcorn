#include "Window.h"
#include "GLAD_Funcs.h"
#include "GLFW_Funcs.h"
#include <iostream>

Window *Window::instance = nullptr;
const char *Window::__dir = nullptr;
GLFWwindow *Window::__glfw_window = nullptr;

void Window::Create(const char *dir) {
  if (!instance) {
    __dir = dir;
    instance = new Window();
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
  std::cout << "WINDOW CREATED" << "\n";

  InitGraphicsLib();
  SetGraphicsViewport();
  StartMainGameLoop();
};

Window::~Window() {
  TerminateWindow();
  __glfw_window = nullptr;

  std::cout << "WINDOW DESTROYED" << "\n";
};

void Window::Init() {
  GLFW_Funcs::Init(GLAD_Funcs::Set_Viewport);
  /* OR ANY OTHER LIB FUNCTION CALL */
}

void Window::CreateWindow() {
  std::cout << "BEFORE: " << __glfw_window << '\n';
  GLFW_Funcs::Create_Window(&__glfw_window, __dir);
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

void Window::StartMainGameLoop() {
  GLFW_Funcs::Main_Game_Loop(__glfw_window);
  /* OR ANY OTHER LIB FUNCTION CALL */
}

void Window::TerminateWindow() {
  GLFW_Funcs::Terminate();
  /* OR ANY OTHER LIB FUNCTION CALL */
}
