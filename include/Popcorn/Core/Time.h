#pragma once

#include "Assert.h"
#include "Global_Macros.h"
#include "Publisher.h"
#include "TimeEvent.h"
#include <iostream>

ENGINE_NAMESPACE_BEGIN
class Time : public Publisher {
public:
  Time()
      : m_now(0.0f),
        // Assumption: First frame took 16.66 ms
        m_delta(16.66f), m_elapsed(0.0f), m_is_time_running(false) {};
  ~Time() = default;

  bool IsTimeRunning() const { return m_is_time_running; };

  void Start() {
    std::cout << "LOOP START";
    PC_ASSERT(!IsTimeRunning(), "GAME LOOP ALREADY RUNNING!");

    m_is_time_running = true;

    while (IsTimeRunning()) {
      TimeEvent te;
      this->PublishEvent(te);
    }
  };

  void Stop() { m_is_time_running = false; };

private:
  float m_now;
  float m_delta;
  float m_elapsed;

  bool m_is_time_running;
};

ENGINE_NAMESPACE_END
