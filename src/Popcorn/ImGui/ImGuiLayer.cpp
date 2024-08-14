#include "Popcorn/ImGui/ImGuiLayer.h"
#include "Event.h"
#include "Global_Macros.h"

#include <imgui.h>

ENGINE_NAMESPACE_BEGIN
ImGuiLayer::ImGuiLayer() {}
ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::OnAttach() {}

void ImGuiLayer::OnDetach() {}

void ImGuiLayer::OnUpdate() {}

void ImGuiLayer::OnEvent(Event &e) {}
ENGINE_NAMESPACE_END
