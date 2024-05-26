#pragma once
#include "Event.h"
#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN

class WindowCloseEvent : public Event {
public:
  WindowCloseEvent()
      : Event{EventType::WindowClose, EventCategory::WindowEvent} {};
  EVENT_OVERRIDE_METHODS(WindowClose)
};
ENGINE_NAMESPACE_END
