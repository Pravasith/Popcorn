#pragma once

#include "Event.h"
#include "Global_Macros.h"
#include "Subscriber.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
class Publisher {
public:
  virtual ~Publisher() = 0;
  static void PublishEvent(Event &e);
  static void Subscribe(const Subscriber *s);
  static void UnSubscribe(const Subscriber *s);

private:
  static std::vector<const Subscriber *> s_subscribers;
};
ENGINE_NAMESPACE_END
