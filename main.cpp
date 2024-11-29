#include <Popcorn.h>
#include <iostream>

int main(int argc, char **argv) {
  Popcorn::Application::Start();
  Popcorn::Application::Run();
  Popcorn::Application::Stop();

  std::cout << "\n";
  if (__cplusplus == 199712L)
    std::cout << "C++98/03\n";
  else if (__cplusplus == 201103L)
    std::cout << "C++11\n";
  else if (__cplusplus == 201402L)
    std::cout << "C++14\n";
  else if (__cplusplus == 201703L)
    std::cout << "C++17\n";
  else if (__cplusplus == 202002L)
    std::cout << "C++20\n";
  else if (__cplusplus == 202302L)
    std::cout << "C++23\n";
  else
    std::cout << "Unknown C++ version\n";

  return 0;
}
