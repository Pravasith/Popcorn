#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"
#include "Popcorn/Core/Helpers.h"
#include <string>
#include <sys/types.h>

ENGINE_NAMESPACE_BEGIN
enum class EventType {
  None = 0,

  // Application Events

  // Window Events
  WindowClose,
  WindowResize,
  FrameBfrResize,

  // Keyboard Events
  KeyPressed,
  KeyReleased,
  KeyTyped,

  // Mouse Events
  MouseButtonPressed,
  MouseButtonReleased,
  MouseMoved,
  MouseScrolled,

  // Time Events
  TimeCPUClockUnit, // One second passed

  // Vulkan Events
  ImageVkLayoutInitialToFinal,
  ImageVkLayoutFinalToInitial
};

enum class EventCategory {
  None = 0,
  ApplicationEvent = shift_l(1),
  WindowEvent = shift_l(2),
  KeyboardEvent = shift_l(3),
  MouseEvent = shift_l(4),
  TimeEvent = shift_l(5),
  VulkanEvent = shift_l(6)
};

// HASH DEFINE START ----------------------------------------------------------
#define EVENT_TYPE_OVERRIDE_METHODS(name)                                      \
  [[nodiscard]] static const EventType GetValueType() {                        \
    return EventType::name;                                                    \
  };                                                                           \
  [[nodiscard]] virtual const EventType GetEventType() const override {        \
    return GetValueType();                                                     \
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
  virtual void PrintDebugData() const {
    PC_PRINT("FROM BASE CLASS -- VIRTUAL METHOD \"PrintDebugData\" NOT DEFINED "
             "IN DERIVED CLASS\n",
             TagType::Print, "EVENT");
  };

private:
  bool m_is_handled = false;
};

class EventDispatcher {
public:
  EventDispatcher(Event &e) : m_event(e) {}

  template <typename T, typename F> bool Dispatch(const F &eventCb) {
    if (T::GetValueType() == m_event.GetEventType()) {
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
