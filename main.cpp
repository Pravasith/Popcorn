#include "Engine.h"
#include "Window.h"
#include <iostream>

int main(int argc, char **argv) {
  Engine::Start();
  Window::Create(argv[0]);
  Window::Destroy();
  Engine::Stop();

  std::cout << "FROM MAIN" << "\n";
  return EXIT_SUCCESS;
}
