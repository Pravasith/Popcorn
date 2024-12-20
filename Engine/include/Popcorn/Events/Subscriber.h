#pragma once

#include "Event.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
class Subscriber {
public:
  void virtual OnEvent(Event &) const = 0;
};
ENGINE_NAMESPACE_END
