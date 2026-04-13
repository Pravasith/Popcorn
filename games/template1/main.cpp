#include <Popcorn/Popcorn.h>
#include <iostream>

int main() {
  std::cout << "ENTRY POINT REACHED!" << '\n';
  Popcorn::Start();
  Popcorn::InitGraphics();
  return 0;
};
