
#include "Popcorn/Core/Time.h"
#include "Assert.h"
#include "Global_Macros.h"
#include "TimeEvent.h"

ENGINE_NAMESPACE_BEGIN
Time *Time::s_instance = nullptr;

bool Time::IsGameLoopRunning() { return m_is_game_loop_running; };

Time *Time::Get() {
  if (!s_instance) {
    s_instance = new Time();
  } else {
    PC_ASSERT(s_instance, "Time class instance exists!");
  };
  return s_instance;
};

void Time::Start() { RunLoop(); };

void Time::RunLoop() {
  while (IsGameLoopRunning()) {
    TimeEvent event;
    this->PublishEvent(event);
  }
};

void Time::Stop() {
  m_is_game_loop_running = false;
  delete s_instance;
  s_instance = nullptr;
};

ENGINE_NAMESPACE_END
