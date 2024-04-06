#pragma once

class Engine {
public:
  static Engine *Get(char *dir);
  static void Destroy();

private:
  Engine();
  ~Engine();

  static Engine *instance;
};
