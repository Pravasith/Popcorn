#pragma once

#include "GlobalMacros.h"
#include "Layer.h"
#include "Popcorn/Events/Event.h"

ENGINE_NAMESPACE_BEGIN
class DebugUIOverlay : public Layer {
public:
  DebugUIOverlay();
  ~DebugUIOverlay();

  void OnAttach() override;
  void OnDetach() override;
  void OnUpdate() override;
  void OnEvent(Event &e) override;

private:
  void *m_os_window;
};

ENGINE_NAMESPACE_END
