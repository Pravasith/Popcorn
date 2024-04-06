#include "Engine.h"
#include <iostream>

int main(int argc, char **argv) {
  Engine::Create();
  /* OR */
  /* Engine::Create(argv[0]); */

  Engine::Destroy();

  std::cout << "FROM MAIN" << "\n";
  return EXIT_SUCCESS;
}
