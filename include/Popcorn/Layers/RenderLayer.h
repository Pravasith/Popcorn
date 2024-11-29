#pragma once

#include "Event.h"
#include "Global_Macros.h"
#include "Layer.h"
#include "Popcorn/Graphics/Renderer.h"

ENGINE_NAMESPACE_BEGIN
class RenderLayer : public Layer {
public:
  RenderLayer();
  ~RenderLayer();

  void OnAttach() override;
  void OnDetach() override;
  void OnUpdate() override;
  void OnEvent(Event &e) override;

private:
  template <RendererType T> static Renderer<T> *s_Renderer;
};

ENGINE_NAMESPACE_END
