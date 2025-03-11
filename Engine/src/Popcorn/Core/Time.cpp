
#include "Time.h"
#include "GlobalMacros.h"
#include "Popcorn/Events/TimeEvent.h"
#include <chrono>

ENGINE_NAMESPACE_BEGIN
Time *Time::s_instance = nullptr;

void Time::Start() {
  auto startTime = std::chrono::high_resolution_clock::now();
  // Temp var
  auto prevFrameTime = startTime;

  while (IsGameLoopRunning()) {
    auto currentTime = std::chrono::high_resolution_clock::now();

    m_elapsed = std::chrono::duration<double, std::chrono::seconds::period>(
                    currentTime - startTime)
                    .count();

    m_delta = std::chrono::duration<double, std::chrono::seconds::period>(
                  currentTime - prevFrameTime)
                  .count();

    prevFrameTime = currentTime;

    //
    // ---------------------------------------------------------------------
    // --- Frame render ----------------------------------------------------
    TimeEvent event{m_elapsed, m_delta};
    this->PublishEvent(event);
    // --- Frame render ----------------------------------------------------
    // ---------------------------------------------------------------------
    //
  }
};

void Time::Stop() {
  m_elapsed = 0;
  m_delta = 0.0166;

  m_isGameLoopRunning = false;
};

ENGINE_NAMESPACE_END
