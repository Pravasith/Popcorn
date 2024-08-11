#include "Window.h"
/* #ifdef IS_WINDOWS_OR_LINUX */
#include "Window_Win64_Linux.h"
/* #else */
/* write_log("Error Importing Platform!"); */
/* #endif */

// SAME -- IF OPEN_GL ...
#include "ImGui_OpenGL_Renderer.h"

ENGINE_NAMESPACE_BEGIN

void *Window::s_platform_window_instance = nullptr;

void Window::Create(const Props &props) {

  /* ---- NOTE ---- */
  /* Multiple window creation not supported at the moment */
  /* So multiple calls to Window::Create will result in  */
  /* same window pointer */
  /* ---- END NOTE ---- */

  /* #ifdef IS_WINDOWS_OR_LINUX */
  s_platform_window_instance = WindowWin64Linux::Init(props);
  /* #else */
  /* write_log("Error Creating Window - Wrong platform!"); */
  /* #endif */

  PC_PRINT_DEBUG("MAIN WINDOW CREATED", 2, "WINDOW");
};

void Window::AddSubscriber(const Subscriber *s) {
  PC_PRINT_DEBUG("WINDOW SUBSCRIPTION ADDED", 2, "WINDOW");

  /* #ifdef IS_WINDOWS_OR_LINUX */
  (static_cast<WindowWin64Linux *>(s_platform_window_instance))->Subscribe(s);
  /* #else */
  /* write_log("Error Creating Window - Wrong platform!"); */
  /* #endif */
}

void Window::Destroy() {
  /* #ifdef IS_WINDOWS_OR_LINUX */
  s_platform_window_instance = nullptr;
  WindowWin64Linux::Terminate();
  /* #else */
  /* write_log("Error Destroying Window - Wrong platform!"); */
  /* #endif */
};

void Window::OnUpdate() {
  /* #ifdef IS_WINDOWS_OR_LINUX */
  (static_cast<WindowWin64Linux *>(s_platform_window_instance))->OnUpdate();
  /* #else */
  /* write_log("Error Creating Window - Wrong platform!"); */
  /* #endif */
};

Window::Window(){
    /* std::cout << "LALALA Window Constructor called" << std::endl; */
};
Window::~Window() { PC_PRINT_DEBUG("MAIN WINDOW DESTROYED", 2, "WINDOW"); };
ENGINE_NAMESPACE_END
