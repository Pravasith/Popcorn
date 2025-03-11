#pragma once

#include "Event.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN
class TimeEvent : public Event {
public:
  TimeEvent(double elapsed, double delta)
      : m_elapsed(elapsed), m_delta(delta) {};

  // Always in Seconds
  [[nodiscard]] double GetDelta() const { return m_delta; };
  // Always in Seconds
  [[nodiscard]] double GetElapsed() const { return m_elapsed; };

  EVENT_CATEGORY_OVERRIDE_METHODS(TimeEvent);
  EVENT_TYPE_OVERRIDE_METHODS(KeyReleased);

  void PrintDebugData() const override {
    PC_PRINT(GetDelta(), TagType::Print, "TIME-EVENT");
  };

private:
  double m_elapsed = 0.;
  double m_delta = 0.01666;
};
ENGINE_NAMESPACE_END
