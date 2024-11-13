#pragma once
#include "Base.h"
#include "Event.h"
#include "Global_Macros.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN

class WindowCloseEvent : public Event {
public:
  WindowCloseEvent() {};

  EVENT_CATEGORY_OVERRIDE_METHODS(WindowEvent)
  EVENT_TYPE_OVERRIDE_METHODS(WindowClose)

  void PrintDebugData() const override {
    PC_PRINT("Window Closed", TagType::Print, "WINDOW-EVENT");
  };
};

class WindowResizeEvent : public Event {
public:
  WindowResizeEvent(const uint16_t w, const uint16_t h)
      : m_window_size({w, h}) {};

  EVENT_CATEGORY_OVERRIDE_METHODS(WindowEvent)
  EVENT_TYPE_OVERRIDE_METHODS(WindowResize)

  void PrintDebugData() const override {
    PC_PRINT("Window Resize" << " W: " << GetWindowSize().width
                             << " H: " << GetWindowSize().height,
             TagType::Print, "WINDOW-EVENT");
  };

  struct WindowSize {
    uint16_t width = 0;
    uint16_t height = 0;
  };

  WindowSize GetWindowSize() const { return m_window_size; };

private:
  WindowSize m_window_size;
};
ENGINE_NAMESPACE_END
