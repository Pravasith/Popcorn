#pragma once

#include <iostream>

namespace Popcorn {
void Popcorn::Start() {
  // test
  std::cout << "Popcorn start" << '\n';
#ifdef PC_DEBUG
  std::cout << "Debug mode" << '\n';
#endif
}
} // namespace Popcorn
