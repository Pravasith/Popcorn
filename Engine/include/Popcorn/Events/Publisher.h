#pragma once

#include "Event.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Subscriber.h"
#include <string>
#include <vector>

ENGINE_NAMESPACE_BEGIN
class Publisher {
public:
  Publisher(std::string name);
  virtual ~Publisher() = 0;

  void PublishEvent(Event &e, bool isBroadcast = false) const;
  void Subscribe(Subscriber *s);
  void UnSubscribe(Subscriber *s);

#ifdef PC_DEBUG
  void PrintSubscribers() const {
    PC_PRINT(m_subscribers.size()
                 << " Subscribers subscribed to: " << m_name << ":",
             TagType::Print, "PUBLISHER SUBSCRIBERS")
    for (Subscriber *s : m_subscribers) {
      PC_PRINT(s, TagType::Print, "PUBLISHER SUBSCRIBERS")
    }
  }

#endif

private:
  std::vector<Subscriber *> m_subscribers{};
  std::string m_name;
};
ENGINE_NAMESPACE_END
