#include "Publisher.h"
#include "Global_Macros.h"
#include <algorithm>
#include <iostream>

ENGINE_NAMESPACE_BEGIN
void Publisher::Subscribe(const Subscriber *subscriber) {
  m_subscribers.push_back(subscriber);
  std::cout << "      --- m_subscribers in Publisher: " << ", Size = "
            << m_subscribers.size() << '\n';
};

void Publisher::Unsubscribe(const Subscriber *subscriber) {
  auto iter = std::find(m_subscribers.begin(), m_subscribers.end(), subscriber);

  if (iter != m_subscribers.end()) {
    m_subscribers.erase(iter);
  };
};

void Publisher::PublishEvent(Event &e) {
  for (const Subscriber *s : m_subscribers) {
    s->OnEvent(e);

    if (e.IsHandled()) {
      break;
    };
  }
};

/* Publisher::Publisher() { */
/*   std::cout << "      --- m_subscribers in Publisher: " << ", Size = " */
/*             << m_subscribers.size() << '\n'; */
/* } */

Publisher::~Publisher() {
  m_subscribers.clear();

  std::cout << "      --- m_subscribers in Publisher: " << ", Size = "
            << m_subscribers.size() << '\n';
}
ENGINE_NAMESPACE_END
