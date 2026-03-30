#pragma once

#include "Base.h"
#include "GlobalMacros.h"
#include "Popcorn/Events/Publisher.h"

ENGINE_NAMESPACE_BEGIN

class Time : public Publisher {
public:
  [[nodiscard]] inline bool IsGameLoopRunning() const {
    return m_isGameLoopRunning;
  }

  [[nodiscard]] inline static Time *Get() {
    if (!s_instance) {
      s_instance = new Time();
    } else {
      PC_WARN("Time class instance exists!");
    }

    return s_instance;
  }

  inline static void Destroy() {
    if (!s_instance) {
      PC_WARN("Time instance already destroyed!");
    } else {
      delete s_instance;
      s_instance = nullptr;
    }
  }

  void Start();
  void Stop();

private:
  Time()
      : Publisher("Time"), m_elapsed(0.0),
        // Assumption: First frame took 16.66 ms
        m_delta(0.01666), m_isGameLoopRunning(true) {
    PC_PRINT("CREATED", TagType::Constr, "TIME")
  }

  ~Time(){PC_PRINT("DESTROYED", TagType::Destr, "TIME")}

  Time(const Time &) = delete;
  Time &operator=(const Time &) = delete;
  Time(const Time &&) = delete;
  Time &operator()(const Time &&) = delete;

  static Time *s_instance;

  double m_elapsed;
  double m_delta;

  bool m_isGameLoopRunning;
};

ENGINE_NAMESPACE_END
