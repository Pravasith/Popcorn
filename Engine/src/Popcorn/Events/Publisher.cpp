#include "Publisher.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Subscriber.h"
#include <algorithm>
#include <vector>

ENGINE_NAMESPACE_BEGIN

Publisher::Publisher(std::string name)
    : m_name(name) {
        PC_PRINT(m_name << " CREATED.", TagType::Constr, "PUBLISHER")
      };

Publisher::~Publisher() {
  m_subscribers.clear();
  PC_PRINT(m_subscribers.size(), TagType::Print, "publisher current count")
  PC_PRINT("DESTROYED", TagType::Destr, "PUBLISHER")
}

void Publisher::Subscribe(Subscriber *subscriber) {
  m_subscribers.push_back(subscriber);
  // PC_PRINT(subscriber->GetName() << " subscribed to " << m_name,
  // TagType::Print,
  //          "PUBLISHERS")
};

void Publisher::UnSubscribe(Subscriber *subscriber) {
  auto iter = std::find(m_subscribers.begin(), m_subscribers.end(), subscriber);
  if (iter != m_subscribers.end()) {
    // PC_PRINT(subscriber->GetName() << " un-subscribed from " << m_name,
    //          TagType::Print, "PUBLISHERS")
    m_subscribers.erase(iter);
  };
};

void Publisher::PublishEvent(Event &e) const {
  for (Subscriber *s : m_subscribers) {
    s->OnEvent(e);

    if (e.IsHandled()) {
      break;
    };
  }
};

ENGINE_NAMESPACE_END
