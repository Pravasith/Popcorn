#pragma once
#include "Event.h"

#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
class KeyPressedEvent : public Event {
public:
  // TODO: ADD CUSTOM KEY CODES
  KeyPressedEvent(const int keyCode, const int keyScanCode)
      // EXPLICITLY INIT BASE CLASS
      : Event(EventType::KeyPressed, EventCategory::KeyboardEvent),
        // INIT MEMBERS
        m_key_code{keyCode}, m_key_scan_code{keyScanCode} {};

  EVENT_OVERRIDE_METHODS(KeyPressed);

  [[nodiscard]] int GetKeyCode() { return m_key_code; };
  [[nodiscard]] int GetKeyScanCode() { return m_key_scan_code; };

  [[nodiscard]] std::string PrintDebugData() const override {
    std::stringstream ss;
    ss << "Key Pressed: " << m_key_code << ", Scan Code: " << m_key_scan_code
       << '\n';
    return ss.str();
  };

private:
  int m_key_code, m_key_scan_code;
};

class KeyReleasedEvent : public Event {
public:
  // TODO: ADD CUSTOM KEY CODES
  KeyReleasedEvent(const int keyCode, const int keyScanCode)
      // EXPLICITLY INIT BASE CLASS
      : Event(EventType::KeyReleased, EventCategory::KeyboardEvent),
        // INIT MEMBERS
        m_key_code{keyCode}, m_key_scan_code{keyScanCode} {};

  EVENT_OVERRIDE_METHODS(KeyReleased);

  [[nodiscard]] int GetKeyCode() { return m_key_code; };
  [[nodiscard]] int GetKeyScanCode() { return m_key_scan_code; };

  [[nodiscard]] std::string PrintDebugData() const override {
    std::stringstream ss;
    ss << "Key Released: " << m_key_code << ", Scan Code: " << m_key_scan_code
       << '\n';
    return ss.str();
  };

private:
  int m_key_code, m_key_scan_code;
};

ENGINE_NAMESPACE_END
