#pragma once

#include "Event.h"
#include "GlobalMacros.h"
#include "Subscriber.h"
#include <string>
#include <vector>

ENGINE_NAMESPACE_BEGIN
class Publisher {
public:
  Publisher(std::string name);
  virtual ~Publisher() = 0;

  void PublishEvent(Event &e) const;
  void Subscribe(Subscriber *s);
  void UnSubscribe(Subscriber *s);

private:
  std::vector<Subscriber *> m_subscribers{};
  std::string m_name;
};
ENGINE_NAMESPACE_END
