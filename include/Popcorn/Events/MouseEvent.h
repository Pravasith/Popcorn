
#pragma once
#include "Event.h"
#include "Global_Macros.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

ENGINE_NAMESPACE_BEGIN

class MouseMovedEvent : public Event {
public:
  MouseMovedEvent(const float xPos, const float yPos)
      : Event{EventType::MouseMoved, EventCategory::MouseEvent}, m_xpos{xPos},
        m_ypos{yPos} {};
  EVENT_OVERRIDE_METHODS(MouseMoved);

  [[nodiscard]] float_t GetXPos() { return m_xpos; };
  [[nodiscard]] float_t GetYPos() { return m_ypos; };

  [[nodiscard]] std::string PrintDebugData() const override {
    std::stringstream ss;
    ss << "X: " << m_xpos << ", Y: " << m_ypos << '\n';

    return ss.str();
  };

private:
  float m_xpos = 0, m_ypos = 0;
};
ENGINE_NAMESPACE_END
