#pragma once

class Engine {
public:
  static void Start();
  static Engine *Get();
  static void Stop();

private:
  Engine();
  ~Engine();

  static Engine *instance;
};
