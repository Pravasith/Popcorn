#include "Window.h"
#include "Window_Win64_Linux.h"
#include <iostream>
#include <ostream>

ENGINE_NAMESPACE_BEGIN
/* #ifdef WINDOWS_PLATFORM */
Window *Window::Create(const Props &props) {
  std::cout << "MAIN WINDOW CREATED LALALA" << std::endl;

  WindowWin64Linux win;
  win.Init(Props());

  return nullptr;
};

/* #endif */
ENGINE_NAMESPACE_END
