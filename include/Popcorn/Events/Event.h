#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <string>
#include <sys/types.h>

ENGINE_NAMESPACE_BEGIN
enum class EventType {
  None = 0,

  // Application Events

  // Window Events
  WindowClose = bit_shift_left(1),
  WindowResize = bit_shift_left(2),

  // Keyboard Events
  KeyPressed = bit_shift_left(3),
  KeyReleased = bit_shift_left(4),
  KeyTyped = bit_shift_left(5),

  // Mouse Events
  MouseButtonPressed = bit_shift_left(6),
  MouseButtonReleased = bit_shift_left(7),
  MouseMoved = bit_shift_left(8),
  MouseScrolled = bit_shift_left(9),
};

enum class EventCategory {
  None = 0,
  ApplicationEvent = bit_shift_left(1),
  WindowEvent = bit_shift_left(2),
  KeyboardEvent = bit_shift_left(3),
  MouseEvent = bit_shift_left(4),
  /* ControllerEvent = bit_shift_left(5), */
};

class Event {
public:
  explicit Event(EventType eventType, EventCategory eventCategory)
      : m_type{eventType}, m_category{eventCategory} {};
  virtual ~Event() = default;

  [[nodiscard]] EventType GetEventType() const { return m_type; };

  bool BelongsToCategory(const EventCategory evtCategory) const {
    return static_cast<uint_fast16_t>(m_category) &
           (static_cast<uint_fast16_t>(evtCategory));
  };

  bool IsHandled() const { return m_is_handled; }
  void SetIsHandled(const bool isHandled) { m_is_handled = isHandled; };

  // -----------------------------------------------------------------------
  // EVENT_OVERRIDE_METHODS Macro --- START -- DEBUG ONLY
  // -----------------------------------------------------------------------
// LOOK IN Global_Macros.h file
#define EVENT_OVERRIDE_METHODS(name)                                           \
  [[nodiscard]] ENUM_TO_STRING(EventType, name);

  [[nodiscard]] virtual const char *GetEventTypeName() const = 0;
  // -----------------------------------------------------------------------
  // EVENT_OVERRIDE_METHODS Macro --- END -- DEBUG ONLY
  // -----------------------------------------------------------------------

  [[nodiscard]] virtual std::string PrintDebugData() const {
    return "From base class -- virtual method \"PrintDebugData\" not defined "
           "in derived class\n";
  };

private:
  EventType m_type;
  EventCategory m_category;
  bool m_is_handled = false;
};

ENGINE_NAMESPACE_END
