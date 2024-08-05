#pragma once

#include "Event.h"
#include "Global_Macros.h"
#include "Layer.h"
#include <cstdint>
#include <vector>

ENGINE_NAMESPACE_BEGIN

class LayerStack {
public:
  LayerStack() = default;
  ~LayerStack();

  void PushLayer(Layer *);
  void PopLayer(Layer *);

  void PushOverlay(Layer *);
  void PopOverlay(Layer *);

  void PropagateEvents(Event &e) {
    for (auto it = m_layer_stack.end(); it < m_layer_stack.begin(); --it) {
      (*it)->OnEvent(e);
      if (e.IsHandled()) {
        break;
      }
    }
  };

  void UpdateLayerStack() {
    for (Layer *l : m_layer_stack) {
      l->OnUpdate();
    }
  };

private:
  std::vector<Layer *> m_layer_stack;
  uint32_t m_separator_index = 0;
};

ENGINE_NAMESPACE_END
