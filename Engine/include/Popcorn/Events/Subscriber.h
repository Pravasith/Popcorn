#pragma once

#include "Event.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN

class Subscriber {
public:
  Subscriber() = default;
  virtual ~Subscriber() = default;

  // Subscriber(std::string name)
  //     : m_name(name) {
  //         PC_PRINT(m_name << " Created.", TagType::Constr, "Subscriber.h")
  //       };
  // virtual ~Subscriber() {
  //   PC_PRINT(m_name << " Destroyed.", TagType::Destr, "Subscriber.h")
  // };

  virtual void OnEvent(Event &) = 0;
};

ENGINE_NAMESPACE_END
