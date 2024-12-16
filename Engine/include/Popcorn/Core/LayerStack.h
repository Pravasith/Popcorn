#pragma once

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

  template <typename F> void IterateBackwards(const F &cb) {
    // for (auto it = m_layer_stack.end() - 1; it >= m_layer_stack.begin();
    // --it) {
    //   PC_PRINT(&it, TagType::Print, "LAYERSTACK")
    //   cb(it);
    // }

    for (auto it = m_layer_stack.rbegin(); it != m_layer_stack.rend(); ++it) {
      cb(it);
    }
  };

  // TODO: REFACTOR WHEN DEALING WITH TIME
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
