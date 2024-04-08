#include "Engine.h"
#include "Window.h"

int main(int argc, char **argv) {
  Engine::Start();
  Window::Create(argv[0]);
  Window::Destroy();
  Engine::Stop();

  return 0;
}
