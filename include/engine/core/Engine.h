#pragma once

/* namespace EngineCore {} */

class Engine {
public:
  static Engine *Get();

private:
  Engine();
  ~Engine();
};
