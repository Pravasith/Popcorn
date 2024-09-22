#pragma once

#include "Application.h"
#include "Assert.h"
#include "Global_Macros.h"
#include "Publisher.h"
#include "TimeEvent.h"

ENGINE_NAMESPACE_BEGIN
class Time : public Publisher {
public:
  Time()
      : m_now(0.0f),
        // Assumption: First frame took 16.66 ms
        m_delta(16.66f), m_elapsed(0.0f) {};
  ~Time() = default;

  void Start() {
    PC_ASSERT(!Application::IsGameLoopRunning(), "GAME LOOP ALREADY RUNNING!");

    while (Application::IsGameLoopRunning()) {
      // this->PublishEvent(TimeEvent<typename T>);
    }
  };

private:
  float m_now;
  float m_delta;
  float m_elapsed;
};

ENGINE_NAMESPACE_END
