#pragma once

class Engine {
public:
  static void Create() { Engine::Create("Engine Window"); };
  static void Create(const char *dir);

  static Engine *Get();

  static void Destroy();

private:
  Engine();
  ~Engine();

  static Engine *instance;
};
