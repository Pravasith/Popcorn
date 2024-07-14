
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
      // EXPLICITLY INIT BASE CLASS
      : Event{EventType::MouseMoved, EventCategory::MouseEvent},
        // INIT MEMBERS
        m_x_pos{xPos}, m_y_pos{yPos} {};

  EVENT_OVERRIDE_METHODS(MouseMoved);

  [[nodiscard]] float_t GetXPos() const { return m_x_pos; };
  [[nodiscard]] float_t GetYPos() const { return m_y_pos; };

  [[nodiscard]] std::string PrintDebugData() const override {
    std::stringstream ss;
    ss << "X: " << m_x_pos << ", Y: " << m_y_pos << '\n';

    return ss.str();
  };

private:
  float m_x_pos = 0, m_y_pos = 0;
};
ENGINE_NAMESPACE_END
