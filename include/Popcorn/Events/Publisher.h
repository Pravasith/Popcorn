#pragma once

#include "Event.h"
#include "Global_Macros.h"
#include "Subscriber.h"
#include <vector>

ENGINE_NAMESPACE_BEGIN
class Publisher {
public:
  virtual ~Publisher() = 0;
  void PublishEvent(const Event &e);
  void Subscribe(const Subscriber *s);
  void Unsubscribe(const Subscriber *s);

private:
  std::vector<const Subscriber *> m_subscribers;
};
ENGINE_NAMESPACE_END
