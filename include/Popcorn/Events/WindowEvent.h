#pragma once
#include "Event.h"
#include "Global_Macros.h"
#include <cstdint>

ENGINE_NAMESPACE_BEGIN

class WindowCloseEvent : public Event {
public:
  WindowCloseEvent(){};

  EVENT_CATEGORY_OVERRIDE_METHODS(WindowEvent)
  EVENT_TYPE_OVERRIDE_METHODS(WindowClose)

  [[nodiscard]] std::string PrintDebugData() const override {
    std::stringstream ss;
    ss << "Window Closed Event." << '\n';
    return ss.str();
  };
};

class WindowResizeEvent : public Event {
public:
  WindowResizeEvent(const uint16_t w, const uint16_t h)
      : m_window_size({w, h}){};

  EVENT_CATEGORY_OVERRIDE_METHODS(WindowEvent)
  EVENT_TYPE_OVERRIDE_METHODS(WindowResize)

  [[nodiscard]] std::string PrintDebugData() const override {
    std::stringstream ss;

    ss << "Window Resize Event: " << '\n'
       << " -- W: " << GetWindowSize().width << '\n'
       << " -- H: " << GetWindowSize().height << '\n';
    return ss.str();
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
