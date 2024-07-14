#pragma once
#include "Event.h"
#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN

class WindowCloseEvent : public Event {
public:
  WindowCloseEvent()
      : Event{EventType::WindowClose, EventCategory::WindowEvent} {};

  EVENT_OVERRIDE_METHODS(WindowClose)

  [[nodiscard]] std::string PrintDebugData() const override {
    std::stringstream ss;
    ss << "Window Closed Event." << '\n';
    return ss.str();
  };
};
ENGINE_NAMESPACE_END
