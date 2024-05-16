#pragma once

#include <string>

class Subscriber {
public:
  Subscriber(const Subscriber &s) = delete;
  Subscriber operator=(const Subscriber &s) = delete;

  void virtual OnEvent(const std::string &s) const;
};
