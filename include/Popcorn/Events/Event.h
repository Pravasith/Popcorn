#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
enum class EventType {
  None = 0,

  // Window Events
  WindowClose,
  WindowResize,

  // Application Events

  // Keyboard Events
  KeyPressed,
  KeyReleased,
  KeyTyped,

  // Mouse Events
  MouseButtonPressed,
  MouseButtonReleased,
  MouseMoved,
  MouseScrolled
};

enum EventCategory {

  KeyboardEvent = bit_shift_left(1),
};

class Event {
public:
  explicit Event(EventType eventType) : m_type(eventType){};
  virtual ~Event() = default;

  [[nodiscard]] virtual EventType GetEventType() const = 0;

private:
  EventType m_type;
};

class WindowEvent : public Event {
  /* explicit WindowEvent(); */
};
ENGINE_NAMESPACE_END
