#pragma once

#include "Event.h"
#include "Global_Macros.h"
#include "Layer.h"

ENGINE_NAMESPACE_BEGIN
class ImGuiLayer : public Layer {
public:
  ImGuiLayer();
  ~ImGuiLayer();

  void OnAttach() override;
  void OnDetach() override;
  void OnUpdate() override;
  void OnEvent(Event &e) override;

private:
  void *m_os_window;
};

ENGINE_NAMESPACE_END
