
#pragma once
#include "Event.h"
#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN

template <typename T> class MouseMovedEvent : public Event {
public:
  MouseMovedEvent(const T x, const T y) : m_coords({x, y}){};

private:
  struct Coords {
    T x = 0;
    T y = 0;
  };

public:
  Coords GetCoords() const { return m_coords; };

  EVENT_CATEGORY_OVERRIDE_METHODS(MouseEvent);
  EVENT_TYPE_OVERRIDE_METHODS(MouseMoved);

  [[nodiscard]] std::string PrintDebugData() const override {
    std::stringstream ss;
    ss << "X: " << GetCoords().x << ", Y: " << GetCoords().y << '\n';
    return ss.str();
  };

private:
  Coords m_coords;
};

ENGINE_NAMESPACE_END
