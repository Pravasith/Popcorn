#pragma once

#include "Global_Macros.h"
#include <string>

ENGINE_NAMESPACE_BEGIN
class Subscriber {
public:
  void virtual OnEvent(const std::string &s) const = 0;
};
ENGINE_NAMESPACE_END
