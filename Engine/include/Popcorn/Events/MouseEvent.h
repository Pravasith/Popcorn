
#pragma once
#include "Event.h"
#include "GlobalMacros.h"
#include "Popcorn/Core/Base.h"

ENGINE_NAMESPACE_BEGIN

template <typename T> //
class MouseMovedEvent : public Event {
public:
  MouseMovedEvent(const T x, const T y) : m_coords({x, y}) {};

private:
  struct Coords {
    T x = 0;
    T y = 0;
  };

public:
  Coords GetCoords() const { return m_coords; };

  EVENT_CATEGORY_OVERRIDE_METHODS(MouseEvent);
  EVENT_TYPE_OVERRIDE_METHODS(MouseMoved);

  void PrintDebugData() const override {
    PC_PRINT("X: " << GetCoords().x << ", Y: " << GetCoords().y, TagType::Print,
             "MOUSE_EVENT");
  };

private:
  Coords m_coords;
};

ENGINE_NAMESPACE_END
