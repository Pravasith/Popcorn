#include <Popcorn.h>

int main(int argc, char **argv) {

  // CREATE A POPCORN WINDOW
  auto windowProps = Popcorn::Window::Props("Triangle App", 500, 500);
  auto AppWin = Popcorn::Window::Create(windowProps);

  // START THE APPLICATION WITH WINDOW
  Popcorn::Application::Start(AppWin);
  Popcorn::Application::Run();
  Popcorn::Application::Stop();

  return 0;
}
