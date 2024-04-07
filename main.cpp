#include "Engine.h"
#include <iostream>

int main(int argc, char **argv) {
  Engine::Start();
  /* OR */
  /* Engine::Create(argv[0]); */

  Engine::Stop();

  std::cout << "FROM MAIN" << "\n";
  return EXIT_SUCCESS;
}
