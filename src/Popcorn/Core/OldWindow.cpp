#include "GLAD_Funcs.h"
// LB
#include "GLFW_Funcs.h"
#include "Window.h"
#include <iostream>

ENGINE_NAMESPACE_BEGIN
Window *Window::s_instance = nullptr;
const char *Window::s_window_name = nullptr;

void Window::Create(const char *windowName) {
  if (!s_instance) {
    s_window_name = windowName;
    s_instance = new Window();
  }
}

Window *Window::Get() { return s_instance; }

void Window::Destroy() {
  if (s_instance) {
    delete s_instance;
    s_instance = nullptr;
  }
}

Window::Window() {
  Init();
  CreateOSWindow();
  std::cout << "WINDOW CREATED" << "\n";

  InitGraphicsLib();
  SetGraphicsViewport();
  StartMainGameLoop();
};

Window::~Window() {
  TerminateWindow();
  m_glfw_window = nullptr;

  std::cout << "WINDOW DESTROYED" << "\n";
};

void Window::Init() {
  GLFW_Funcs::Init(GLAD_Funcs::Set_Viewport);
  /* OR ANY OTHER LIB FUNCTION CALL */
}

void Window::CreateOSWindow() {
  GLFW_Funcs::Create_Window(&m_glfw_window, s_window_name);
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
  GLFW_Funcs::Main_Game_Loop(m_glfw_window);
  /* OR ANY OTHER LIB FUNCTION CALL */
}

void Window::TerminateWindow() {
  GLFW_Funcs::Terminate();
  /* OR ANY OTHER LIB FUNCTION CALL */
}
ENGINE_NAMESPACE_END
