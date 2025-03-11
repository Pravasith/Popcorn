
#include "Layer.h"
#include "GlobalMacros.h"

ENGINE_NAMESPACE_BEGIN
void Layer::OnAttach() {};
void Layer::OnDetach() {};
bool Layer::OnEvent(Event &) { return false; };
void Layer::OnRender() {};

ENGINE_NAMESPACE_END
