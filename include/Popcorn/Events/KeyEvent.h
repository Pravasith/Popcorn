
#pragma once
#include "Event.h"

#include "Global_Macros.h"
#include "KeyCodes.h"
#include "Popcorn/Core/Base.h"

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

  void PrintDebugData() const override {
    PC_PRINT_DEBUG("Key Pressed: " << GetKeyCode(), 1, "KEY_EVENT");
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

  void PrintDebugData() const override {
    PC_PRINT_DEBUG("Key Released: " << GetKeyCode(), 1, "KEY_EVENT");
  };

private:
  bool m_is_repeat = false;
};

ENGINE_NAMESPACE_END
