#pragma once

#include "Global_Macros.h"
#include "Subscriber.h"

ENGINE_NAMESPACE_BEGIN

class Layer : public Subscriber {
public:
  virtual ~Layer() = default;

  virtual void OnAttach();
  virtual void OnDetach();
  virtual void OnUpdate() = 0;

  void OnEvent(const Event &) const override;
};

ENGINE_NAMESPACE_END
