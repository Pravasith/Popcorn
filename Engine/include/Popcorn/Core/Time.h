#pragma once

#include "Base.h"
#include "GlobalMacros.h"
#include "Popcorn/Events/Publisher.h"

ENGINE_NAMESPACE_BEGIN

class Time : public Publisher {
public:
  Time(const Time &) = delete;
  Time operator()(const Time &) = delete;

  Time(const Time &&) = delete;
  Time operator()(const Time &&) = delete;

  [[nodiscard]] bool IsGameLoopRunning();

  static Time *Get();

  void Start();
  void Stop();

private:
  Time()
      : m_now(0.0f),
        // Assumption: First frame took 16.66 ms
        m_delta(16.66f), m_elapsed(0.0f), m_is_game_loop_running(true) {
          PC_PRINT("CREATED", TagType::Constr, "TIME")
        };
  ~Time() { PC_PRINT("DESTROYED", TagType::Destr, "TIME") };

  float m_now;
  float m_delta;
  float m_elapsed;

  bool m_is_game_loop_running;
  static Time *s_instance;

  void RunLoop();
};

ENGINE_NAMESPACE_END
