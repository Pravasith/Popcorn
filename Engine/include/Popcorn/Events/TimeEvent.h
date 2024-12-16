#pragma once

#include "Event.h"
#include "Global_Macros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
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

  EVENT_CATEGORY_OVERRIDE_METHODS(TimeEvent);
  EVENT_TYPE_OVERRIDE_METHODS(KeyReleased);

  void PrintDebugData() const override {
    PC_PRINT(GetDeltaMS(), TagType::Print, "TIME-EVENT");
  };

private:
  float m_now;
  float m_delta;
  float m_elapsed;
};
ENGINE_NAMESPACE_END
