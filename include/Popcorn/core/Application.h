#pragma once

#include "Global_Macros.h"
#include "Window.h"

ENGINE_NAMESPACE_BEGIN

class Application {
public:
  static void Start();
  static Application *Get();
  static void Stop();

private:
  Application();
  ~Application();

  static Application *instance;
  static Window *window;
};

ENGINE_NAMESPACE_END
