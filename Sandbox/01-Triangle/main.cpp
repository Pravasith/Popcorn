#include <Popcorn.h>
#include <Popcorn/Graphics/Renderer.h>

int main(int argc, char **argv) {

  // CREATE A POPCORN WINDOW
  auto windowProps = Popcorn::Window::Props("Triangle App", 500, 500);
  auto AppWin = Popcorn::Window::Create(windowProps);

  auto x = Popcorn::LayerStack();
  auto y = Popcorn::Renderer<Popcorn::RendererType::Vulkan>::Create(*AppWin);

  // START THE APPLICATION WITH WINDOW
  Popcorn::Application::Start(AppWin);
  Popcorn::Application::Run();
  Popcorn::Application::Stop();

  return 0;
}
