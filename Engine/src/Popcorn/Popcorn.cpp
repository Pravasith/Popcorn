#pragma once

#include <Popcorn/Application.h>
#include <Popcorn/Graphics.h>
#include <iostream>

namespace Popcorn {
void Popcorn::Start() {
  // test
  std::cout << "Popcorn start" << '\n';
  Application *app = Application::Get();

#ifdef PC_DEBUG
  std::cout << "Debug mode" << '\n';
#endif
}

void Popcorn::InitGraphics() {
  // test
  Graphics *gfx = Graphics::Get();
}
} // namespace Popcorn
