#include "Application.h"

namespace Popcorn {
Application *Application::s_instance = nullptr;

Application *Application::Get() {
  if (s_instance) {
    return s_instance;
  }
  return new Application();
}

Application::Application() { std::cout << "APPLICATION CREATED" << '\n'; }
Application::~Application() {
  if (s_instance) {
    delete s_instance;
  }
}
} // namespace Popcorn
