#pragma once

class Window {
public:
  static Window *Get(char *dir);
  static void Destroy();

private:
  Window();
  ~Window();

  static Window *instance;
};
