#include "Window.h"
#include <iostream>

Window *Window::instance = nullptr;

Window *Window::Get(char *dir) {
  /* std::cout << "BEFORE:" << instance << "\n"; */
  if (!instance) {
    instance = new Window();
  }
  /* std::cout << "AFTER:" << instance << "\n"; */

  return instance;
}

void Window::Destroy() {
  if (instance) {
    delete instance;
    instance = nullptr;
  }
}

Window::Window() { std::cout << "ENGINE STARTED" << "\n"; };
Window::~Window() { std::cout << "ENGINE STOPPED" << "\n"; };
