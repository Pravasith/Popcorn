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

enum class EventCategory : unsigned int {
  None = 0,
  ApplicationEvent = bit_shift_left(1),
  WindowEvent = bit_shift_left(2),
  KeyboardEvent = bit_shift_left(3),
  MouseEvent = bit_shift_left(4),
  /* ControllerEvent = bit_shift_left(4), */
};

class Event {
public:
  explicit Event(EventType eventType) : m_type(eventType){};
  virtual ~Event() = default;

  [[nodiscard]] EventType GetEventType() const { return m_type; };
  // Debug only
  // LOOK IN Global_Macros.h file
  /* [[nodiscard]] const char *GetEventName() const { */
  /*  */
  /* }; */

private:
  EventType m_type = EventType::None;
};

class WindowEvent : public Event {
  /* explicit WindowEvent(); */
};
ENGINE_NAMESPACE_END
