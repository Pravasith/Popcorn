#include "Window.h"
#include "Window_Win64_Linux.h"
#include <iostream>
#include <ostream>

ENGINE_NAMESPACE_BEGIN

Window *Window::Create(const Props &props) {
  std::cout << "\n---- MAIN WINDOW CREATED: " << props.Title << " ----"
            << std::endl;
  /* #ifdef IS_WINDOWS_OR_LINUX */
  auto win = WindowWin64Linux::Init(props);
  if (win)
    return win;

  return nullptr;
  /* #else */
  /* write_log("Error Creating Window - Wrong platform!"); */
  /* #endif */
};

void Window::Destroy() {
  /* #ifdef IS_WINDOWS_OR_LINUX */
  WindowWin64Linux::Terminate();
  /* #else */
  /* write_log("Error Creating Window - Wrong platform!"); */
  /* #endif */
};

void Window::Loop() {};

Window::Window(){
    /* std::cout << "LALALA Window Constructor called" << std::endl; */
};
Window::~Window() {
  std::cout << "--- MAIN WINDOW DESTROYED ---\n" << std::endl;
};
ENGINE_NAMESPACE_END
