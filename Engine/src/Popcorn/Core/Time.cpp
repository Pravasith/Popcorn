
#include "Time.h"
#include "GlobalMacros.h"
#include "Popcorn/Events/TimeEvent.h"

ENGINE_NAMESPACE_BEGIN
Time *Time::s_instance = nullptr;

void Time::Start() {
  while (IsGameLoopRunning()) {
    TimeEvent event;
    this->PublishEvent(event);
  }
};

void Time::Stop() {
  m_now = 0;
  m_delta = 16.66;
  m_elapsed = 0;

  m_is_game_loop_running = false;
};

ENGINE_NAMESPACE_END
