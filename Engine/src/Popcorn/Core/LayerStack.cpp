#include "LayerStack.h"
#include <algorithm>

ENGINE_NAMESPACE_BEGIN

LayerStack::~LayerStack() {
  // CLEAN UP AND FREE RESOURCES
  for (Layer *l : m_layer_stack) {
    l->OnDetach();
    delete l;
    l = nullptr;
  }

  m_layer_stack.clear();
};

void LayerStack::PushLayer(Layer *l) {
  m_layer_stack.emplace(m_layer_stack.begin() + m_separator_index++, l);
};

void LayerStack::PopLayer(Layer *l) {
  auto it = std::find(m_layer_stack.begin(),
                      m_layer_stack.begin() + m_separator_index, l);

  if (it != m_layer_stack.begin() + m_separator_index) {
    l->OnDetach();
    m_layer_stack.erase(it);
    --m_separator_index;
  }
}

void LayerStack::PushOverlay(Layer *ol) { m_layer_stack.emplace_back(ol); }

void LayerStack::PopOverlay(Layer *ol) {
  auto it = std::find(m_layer_stack.begin() + m_separator_index,
                      m_layer_stack.end(), ol);

  if (it != m_layer_stack.end()) {
    ol->OnDetach();
    m_layer_stack.erase(it);
  }
}

ENGINE_NAMESPACE_END
