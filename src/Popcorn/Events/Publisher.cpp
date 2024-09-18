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
  PC_PRINT_DEBUG(s_subscribers.size() << " SUBSCRIBED", 8, "PUBLISHERS")
};

void Publisher::UnSubscribe(const Subscriber *subscriber) {
  auto iter = std::find(s_subscribers.begin(), s_subscribers.end(), subscriber);

  if (iter != s_subscribers.end()) {
    s_subscribers.erase(iter);
  };

  PC_PRINT_DEBUG(s_subscribers.size() << " UNSUBSCRIBED", 8, "PUBLISHERS")
};

void Publisher::PublishEvent(Event &e) {
  for (const Subscriber *s : s_subscribers) {
    s->OnEvent(e);

    if (e.IsHandled()) {
      break;
    };
  }
};

Publisher::~Publisher() {
  s_subscribers.clear();
  // PC_PRINT_DEBUG(s_subscribers.size() << " PUBLISHER DESTROYED", 8,
  //                "PUBLISHERS")
}
ENGINE_NAMESPACE_END
