#pragma once

namespace Popcorn {
class Graphics {
public:
  static Graphics *Get();
  static void Destroy();

private:
  Graphics();
  ~Graphics();

private:
  static Graphics *s_instance;
};
} // namespace Popcorn
