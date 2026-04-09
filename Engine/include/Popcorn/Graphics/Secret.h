#pragma once

#include <iostream>
namespace Popcorn {
class Secret {
public:
  Secret() { std::cout << "I'm a private class" << '\n'; };
};
} // namespace Popcorn
