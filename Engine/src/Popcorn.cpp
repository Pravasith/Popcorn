#include <Popcorn/Core/Application.h>
#include <Popcorn/Graphics/Graphics.h>
#include <iostream>

namespace Popcorn {
void Start() {
  // test
  std::cout << "Popcorn start" << '\n';
  Application *app = Application::Get();

#ifdef PC_DEBUG
  std::cout << "Debug mode" << '\n';
#endif
}

void InitGraphics() {
  // test
  Graphics *gfx = Graphics::Get();
}
} // namespace Popcorn
