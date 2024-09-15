#pragma once

#include "Base.h"
#include "Global_Macros.h"
#include "KeyCodes.h"

ENGINE_NAMESPACE_BEGIN

class Input {
public:
  // static Input &Get() {
  //   if (!s_instance)
  //     s_instance = new Input();
  //
  //
  //   return *s_instance;
  // };

  static bool IsKeyPressed(Key::KeyCode kc) {
    return s_instance->IsKeyPressedImpl(kc);
  };

private:
  virtual bool IsKeyPressedImpl(Key::KeyCode kc) = 0;

private:
  Input() { PC_PRINT_DEBUG("INPUT CLASS CONSTRUCTOR", 1, "INPUT.H"); };
  ~Input() {
    delete s_instance;
    PC_PRINT_DEBUG("INPUT CLASS DESTRUCTOR", 1, "INPUT.H");
  };

  static Input *s_instance;
};

ENGINE_NAMESPACE_END
