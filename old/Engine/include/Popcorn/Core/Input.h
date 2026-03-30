#pragma once

#include "Base.h"
#include "GlobalMacros.h"
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
  Input() { PC_PRINT("CREATED", TagType::Constr, "INPUT"); };
  ~Input() {
    // delete s_instance;
    PC_PRINT("DESTROYED", TagType::Destr, "INPUT");
  };

  static Input *s_instance;
};

ENGINE_NAMESPACE_END
