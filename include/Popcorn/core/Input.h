#pragma once

#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN

class Input {
public:
  static void Start();
  static Input *Get();
  static void Stop();

private:
  Input();
  ~Input();

  static Input *s_instance;
};

ENGINE_NAMESPACE_END
