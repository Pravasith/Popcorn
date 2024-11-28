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
  const Renderer *m_Renderer;
};

ENGINE_NAMESPACE_END
