#pragma once

#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
enum class Event_T {
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

class Event {
public:
  explicit Event(Event_T eventType) : m_type(eventType){};
  virtual ~Event() = default;

  [[nodiscard]] virtual Event_T GetEventType() const = 0;

private:
  Event_T m_type;
};

class WindowEvent : public Event {
  /* explicit WindowEvent(); */
};
ENGINE_NAMESPACE_END
