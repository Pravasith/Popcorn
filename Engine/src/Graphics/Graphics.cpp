#include "Popcorn/Graphics/Graphics.h"
#include <iostream>

namespace Popcorn {
Graphics *Graphics::s_instance = nullptr;

Graphics *Graphics::Get() {
  if (s_instance) {
    return s_instance;
  }
  return new Graphics();
}

void Graphics::Destroy() {
  if (s_instance) {
    delete s_instance;
    return;
  }
  std::cout << "Warning: Singleton already destroyed" << '\n';
}

Graphics::Graphics() { std::cout << "GRAPHICS CREATED" << '\n'; }
Graphics::~Graphics() {}
} // namespace Popcorn
