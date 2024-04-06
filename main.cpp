#include "Engine.h"
/* #include "include/engine/core/Engine.h" */
#include <iostream>

int main() {
  Engine::Get();
  std::cout << "MAIN FILE" << "\n";
  return EXIT_SUCCESS;
}
