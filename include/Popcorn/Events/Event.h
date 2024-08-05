#pragma once

#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"
#include <string>
#include <sys/types.h>

ENGINE_NAMESPACE_BEGIN
enum class EventType {
  None = 0,

  // Application Events

  // Window Events
  WindowClose,
  WindowResize,

  // Keyboard Events
  KeyPressed,
  KeyReleased,
  KeyTyped,

  // Mouse Events
  MouseButtonPressed,
  MouseButtonReleased,
  MouseMoved,
  MouseScrolled,
};

enum class EventCategory {
  None = 0,
  ApplicationEvent = bit_shift_left(1),
  WindowEvent = bit_shift_left(2),
  KeyboardEvent = bit_shift_left(3),
  MouseEvent = bit_shift_left(4),
  /* ControllerEvent = bit_shift_left(5), */
};

// HASH DEFINE START ----------------------------------------------------------
#define EVENT_TYPE_OVERRIDE_METHODS(name)                                      \
  [[nodiscard]] static const EventType GetStaticEventType() {                  \
    return EventType::name;                                                    \
  };                                                                           \
  [[nodiscard]] virtual const EventType GetEventType() const override {        \
    return GetStaticEventType();                                               \
  };                                                                           \
  [[nodiscard]] ENUM_TO_STRING(EventType, name);

#define EVENT_CATEGORY_OVERRIDE_METHODS(category)                              \
  [[nodiscard]] const int GetEvtCategory() const override {                    \
    return static_cast<int>(EventCategory::category);                          \
  };
// HASH DEFINE END -------------------------------------------------------------

class Event {
public:
  virtual ~Event() = default;

  [[nodiscard]] virtual const EventType GetEventType() const = 0;
  [[nodiscard]] virtual const int GetEvtCategory() const = 0;

  bool IsHandled() const { return m_is_handled; }
  void SetIsHandled(const bool isHandled) { m_is_handled = isHandled; };

  bool BelongsToCategory(const EventCategory evtCategory) const {
    return static_cast<int>(evtCategory) & GetEvtCategory();
  };

  // DEBUG ONLY
  [[nodiscard]] virtual const char *GetEventTypeName() const = 0;
  // DEBUG ONLY
  [[nodiscard]] virtual std::string PrintDebugData() const {
    return "From base class -- virtual method \"PrintDebugData\" not defined "
           "in derived class\n";
  };

private:
  bool m_is_handled = false;
};

class EventDispatcher {
public:
  EventDispatcher(Event &e) : m_event(e) {}

  template <typename T, typename F> bool Dispatch(F &&eventCb) {
    if (T::GetStaticEventType() == m_event.GetEventType()) {
      m_event.SetIsHandled(
          // m_event.IsHandled() |
          eventCb(static_cast<T &>(m_event)));
      return true;
    }

    return false;
  };

private:
  Event &m_event;
};

ENGINE_NAMESPACE_END
