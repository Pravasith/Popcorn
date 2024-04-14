#pragma once

class Application {
public:
  static void Start();
  static Application *Get();
  static void Stop();

private:
  Application();
  ~Application();

  static Application *instance;
};
