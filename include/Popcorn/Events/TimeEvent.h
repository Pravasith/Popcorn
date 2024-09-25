#pragma once

#include "Event.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
template <typename T> //
class TimeEvent : public Event {
public:
  TimeEvent()
      : m_now(0.0f),
        // Assumption: First frame took 16.66 ms
        m_delta(16.66f), m_elapsed(0.0f) {};
  // Always in MS
  [[nodiscard]] float GetNowMS() const { return m_now; };
  [[nodiscard]] float GetDeltaMS() const { return m_delta; };
  [[nodiscard]] float GetElapsedMS() const { return m_elapsed; };

  void PrintDebugData() const override {
    PC_PRINT_DEBUG(GetDeltaMS(), 1, "TIME");
  };

private:
  float m_now;
  float m_delta;
  float m_elapsed;
};
ENGINE_NAMESPACE_END
