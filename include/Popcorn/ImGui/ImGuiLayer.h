#pragma once

#include "Event.h"
#include "Global_Macros.h"
#include "Layer.h"

ENGINE_NAMESPACE_BEGIN
class ImGuiLayer : public Layer {
  ImGuiLayer();
  ~ImGuiLayer();

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  virtual void OnUpdate() override;
  virtual void OnEvent(Event &) override;
};

ENGINE_NAMESPACE_END
