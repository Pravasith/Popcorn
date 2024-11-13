#include "Publisher.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include "Subscriber.h"
#include <algorithm>
#include <vector>

ENGINE_NAMESPACE_BEGIN

std::vector<const Subscriber *> Publisher::s_subscribers;

void Publisher::Subscribe(const Subscriber *subscriber) {
  s_subscribers.push_back(subscriber);
  PC_PRINT("SUBSCRIBED", TagType::Print, "PUBLISHERS")
};

void Publisher::UnSubscribe(const Subscriber *subscriber) {
  auto iter = std::find(s_subscribers.begin(), s_subscribers.end(), subscriber);

  if (iter != s_subscribers.end()) {
    s_subscribers.erase(iter);
  };
  PC_PRINT("UNSUBSCRIBED", TagType::Print, "PUBLISHERS")
};

void Publisher::PublishEvent(Event &e) {
  for (const Subscriber *s : s_subscribers) {
    s->OnEvent(e);

    if (e.IsHandled()) {
      break;
    };
  }
};

Publisher::Publisher() {
  PC_PRINT("CREATED", TagType::Constr, "PUBLISHER")

      PC_PRINT("PUBLISHERS SIZE: " << s_subscribers.size(), TagType::Print,
               "PUBLISHER")
};

Publisher::~Publisher() {
  s_subscribers.clear();
  PC_PRINT("PUBLISHERS SIZE: " << s_subscribers.size(), TagType::Print,
           "PUBLISHER")
  PC_PRINT("DESTROYED", TagType::Destr, "PUBLISHER")
}
ENGINE_NAMESPACE_END
