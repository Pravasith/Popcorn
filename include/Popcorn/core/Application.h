#pragma once
#include "Window.h"

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
