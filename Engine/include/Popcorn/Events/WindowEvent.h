#pragma once
#include "Base.h"
#include "Event.h"
#include "GlobalMacros.h"
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
  WindowResizeEvent(const uint32_t w, const uint32_t h)
      : m_window_size({w, h}) {};

  EVENT_CATEGORY_OVERRIDE_METHODS(WindowEvent)
  EVENT_TYPE_OVERRIDE_METHODS(WindowResize)

  void PrintDebugData() const override {
    PC_PRINT("Window Resize" << " W: " << GetWindowSize().width
                             << " H: " << GetWindowSize().height,
             TagType::Print, "WINDOW-EVENT");
  };

  struct WindowSize {
    uint32_t width = 0;
    uint32_t height = 0;
  };

  WindowSize GetWindowSize() const { return m_window_size; };

private:
  WindowSize m_window_size;
};

class FrameBfrResizeEvent : public Event {
private:
  struct FrameBfrSize {
    uint32_t width = 0;
    uint32_t height = 0;
  };

public:
  FrameBfrResizeEvent(const uint32_t w, const uint32_t h)
      : m_window_size({w, h}) {};

  EVENT_CATEGORY_OVERRIDE_METHODS(WindowEvent)
  EVENT_TYPE_OVERRIDE_METHODS(FrameBfrResize)

  void PrintDebugData() const override {
    PC_PRINT("FRAMEBFR RESIZE" << " W: " << GetFrameBfrSize().width
                               << " H: " << GetFrameBfrSize().height,
             TagType::Print, "WINDOW-EVENT");
  };

  FrameBfrSize GetFrameBfrSize() const { return m_window_size; };

private:
  FrameBfrSize m_window_size;
};

ENGINE_NAMESPACE_END
