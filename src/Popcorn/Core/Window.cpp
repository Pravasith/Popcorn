#include "Window.h"
#include "Window_Win64_Linux.h"
#include <iostream>
#include <ostream>

ENGINE_NAMESPACE_BEGIN
Window *Window::s_window_instance = nullptr;

Window *Window::Create(const Props &props) {
  std::cout << "MAIN WINDOW CREATED LALALA" << std::endl;
  /* #ifdef IS_WINDOWS_OR_LINUX */
  return WindowWin64Linux::Get(props);
  /* #else */
  /* write_log("Error Creating Window - Wrong platform!"); */
  /* #endif */

  return nullptr;
};

Window::Window() {
  std::cout << "LALALA Window Constructor called" << std::endl;
};
Window::~Window() { std::cout << "MAIN WINDOW DESTROYED LALALA" << std::endl; };
ENGINE_NAMESPACE_END
