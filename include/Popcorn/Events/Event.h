#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <cstdint>
#include <string>
#include <sys/types.h>

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

enum class EventCategory {
  None = 0,
  ApplicationEvent = bit_shift_left(1),
  WindowEvent = bit_shift_left(2),
  KeyboardEvent = bit_shift_left(3),
  MouseEvent = bit_shift_left(4),
  /* ControllerEvent = bit_shift_left(5), */
};

// Debug only
// LOOK IN Global_Macros.h file
#define EVENT_OVERRIDE_METHODS(name)                                           \
  [[nodiscard]] ENUM_TO_STRING(EventType, name);

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

  // DEBUG ONLY
  [[nodiscard]] virtual const char *GetEventTypeName() const = 0;
  [[nodiscard]] virtual std::string PrintDebugData() const {
    return "From base class -- virtual method \"PrintDebugData\" not defined "
           "in derived class\n";
  };

private:
  EventType m_type;
  EventCategory m_category;
};

ENGINE_NAMESPACE_END
