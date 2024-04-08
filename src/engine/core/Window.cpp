#include "Window.h"
#include <iostream>

Window *Window::instance = nullptr;

void Window::Create(const char *dir) {
  std::cout << dir << '\n';
  if (!instance) {
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

Window::Window() { std::cout << "WINDOW CREATED" << "\n"; };
Window::~Window() { std::cout << "WINDOW DESTROYED" << "\n"; };
