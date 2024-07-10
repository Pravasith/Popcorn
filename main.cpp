#include <Popcorn.h>

int main(int argc, char **argv) {
  Popcorn::Application::Start();
  Popcorn::Application::Run();
  Popcorn::Application::Stop();

  return 0;
}
