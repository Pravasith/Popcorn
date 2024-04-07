#pragma once

class Window {
public:
  static void Create() { Window::Create("Game Window"); };
  static void Create(const char *dir);

  static Window *Get();

  static void Destroy();

private:
  Window();
  ~Window();

  static Window *instance;
};
