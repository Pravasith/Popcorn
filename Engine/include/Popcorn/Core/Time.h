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

  [[nodiscard]] inline bool IsGameLoopRunning() const {
    return m_is_game_loop_running;
  };

  [[nodiscard]] inline static Time *Create() {
    if (!s_instance) {
      s_instance = new Time();
    } else {
      PC_WARN("Time class instance exists!");
    };
    return s_instance;
  };

  inline static void Destroy() {
    if (!s_instance) {
      PC_WARN("Time instance already destroyed!");
    } else {
      delete s_instance;
      s_instance = nullptr;
    };
  };

  void Start();
  void Stop();

private:
  Time()
      : m_now(0.0),
        // Assumption: First frame took 16.66 ms
        m_delta(16.66), m_elapsed(0.0), m_is_game_loop_running(true) {
          PC_PRINT("CREATED", TagType::Constr, "TIME")
        };
  ~Time() { PC_PRINT("DESTROYED", TagType::Destr, "TIME") };

  static Time *s_instance;

  double m_now;
  double m_delta;
  double m_elapsed;

  bool m_is_game_loop_running;
};

ENGINE_NAMESPACE_END
