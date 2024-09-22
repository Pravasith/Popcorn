
#pragma once
#include "Event.h"

#include "Global_Macros.h"
#include "KeyCodes.h"

ENGINE_NAMESPACE_BEGIN
using namespace Key;

class KeyEvent : public Event {
public:
  KeyEvent() = delete;
  virtual ~KeyEvent() = default;

  [[nodiscard]] const KeyCode GetKeyCode() const { return m_key_code; };

  EVENT_CATEGORY_OVERRIDE_METHODS(KeyboardEvent);

protected:
  KeyEvent(const KeyCode keyCode)
      // EXPLICITLY INIT BASE CLASS
      : m_key_code{keyCode} {};

private:
  KeyCode m_key_code;
};

class KeyPressedEvent : public KeyEvent {
public:
  KeyPressedEvent(const KeyCode keyCode, bool isRepeat = false)
      : KeyEvent(keyCode), m_is_repeat(isRepeat) {};

  bool IsRepeat() const { return m_is_repeat; }

  EVENT_TYPE_OVERRIDE_METHODS(KeyPressed);

  [[nodiscard]] std::string PrintDebugData() const override {
    std::stringstream ss;

    ss << "Key Pressed: " << GetKeyCode() << '\n';
    return ss.str();
  };

private:
  bool m_is_repeat = false;
};

class KeyReleasedEvent : public KeyEvent {
public:
  KeyReleasedEvent(const KeyCode keyCode, bool isRepeat = false)
      : KeyEvent(keyCode), m_is_repeat(isRepeat) {};

  bool IsRepeat() const { return m_is_repeat; }

  EVENT_TYPE_OVERRIDE_METHODS(KeyReleased);

  [[nodiscard]] std::string PrintDebugData() const override {
    std::stringstream ss;

    ss << "Key Released: " << GetKeyCode() << '\n';
    return ss.str();
  };

private:
  bool m_is_repeat = false;
};

ENGINE_NAMESPACE_END
