#pragma once

#include "Event.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "TimeEvent.h"
#include <string>

ENGINE_NAMESPACE_BEGIN
class Subscriber {
public:
  Subscriber(std::string name)
      : m_name(name) {
          PC_PRINT(m_name << " Created.", TagType::Constr, "Subscriber.h")
        };
  virtual ~Subscriber() {
    PC_PRINT(m_name << " Destroyed.", TagType::Destr, "Subscriber.h")
  };

  std::string GetName() { return m_name; };

  virtual void OnEvent(Event &) = 0;
  virtual bool OnUpdate(TimeEvent &) = 0;

private:
  std::string m_name;
};
ENGINE_NAMESPACE_END
