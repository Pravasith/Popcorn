#pragma once

namespace Popcorn {
class Application {
public:
  static Application *Get();
  static void Destroy();

private:
  Application();
  ~Application();

private:
  static Application *s_instance;
};
} // namespace Popcorn
