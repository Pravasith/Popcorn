#pragma once

#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
class Application {
public:
  static void Start();
  static Application *Get();
  static void Stop();

private:
  Application();
  ~Application();

  static Application *s_instance;
};
ENGINE_NAMESPACE_END
