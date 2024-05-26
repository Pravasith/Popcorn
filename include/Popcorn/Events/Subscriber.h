#pragma once

#include "Event.h"
#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
class Subscriber {
public:
  void virtual OnEvent(const Event &s) const = 0;
};
ENGINE_NAMESPACE_END
