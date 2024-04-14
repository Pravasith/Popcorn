#pragma once

class Input {
public:
  static void Start();
  static Input *Get();
  static void Stop();

private:
  Input();
  ~Input();

  static Input *instance;
};
