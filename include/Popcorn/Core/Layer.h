#pragma once

#include "Event.h"
#include "Global_Macros.h"

ENGINE_NAMESPACE_BEGIN
class Layer {
public:
  virtual ~Layer() = default;

  virtual void OnAttach();
  virtual void OnDetach();
  virtual void OnUpdate() = 0;
  virtual void OnEvent(Event &e);
};

ENGINE_NAMESPACE_END
