#include "Publisher.h"
#include <algorithm>
#include <iostream>
#include <string>

void Publisher::Subscribe(const Subscriber *subscriber) {
  m_subscribers.push_back(subscriber);
};

void Publisher::Unsubscribe(const Subscriber *subscriber) {
  auto iter = std::find(m_subscribers.begin(), m_subscribers.end(), subscriber);

  if (iter != m_subscribers.end()) {
    m_subscribers.erase(iter);
  };
};

void Publisher::PublishEvent(const std::string &e) {
  for (const Subscriber *s : m_subscribers) {
    s->OnEvent(e);
  }
};

Publisher::Publisher() {
  std::cout << "      --- m_subscribers in Publisher: " << ", Size = "
            << m_subscribers.size() << '\n';
}

Publisher::~Publisher() {
  m_subscribers.clear();

  std::cout << "      --- m_subscribers in Publisher: " << ", Size = "
            << m_subscribers.size() << '\n';
}
