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

Graphics::Graphics() { std::cout << "GRAPHICS CREATED" << '\n'; }
Graphics::~Graphics() {
  if (s_instance) {
    delete s_instance;
  }
}
} // namespace Popcorn
