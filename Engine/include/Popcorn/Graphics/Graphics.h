#pragma once

namespace Popcorn {
class Graphics {
public:
  static Graphics *Get();

private:
  Graphics();
  ~Graphics();

private:
  static Graphics *s_instance;
};
} // namespace Popcorn
