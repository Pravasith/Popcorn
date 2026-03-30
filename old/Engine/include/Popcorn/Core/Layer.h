#pragma once

#include "GlobalMacros.h"
#include "Popcorn/Events/Event.h"
#include "TimeEvent.h"

ENGINE_NAMESPACE_BEGIN
class Layer {
public:
  virtual ~Layer() = default;

  virtual void OnAttach();
  virtual void OnDetach();

  virtual bool OnEvent(Event &e);
  virtual void OnUpdate(TimeEvent &e) = 0;
  virtual void OnRender();
};

ENGINE_NAMESPACE_END
