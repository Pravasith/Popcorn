#include "Engine.h"
#include <iostream>

int main(int argc, char **argv) {
  Engine::Get(argv[0]);
  Engine::Destroy();

  std::cout << "FROM MAIN" << "\n";
  return EXIT_SUCCESS;
}
