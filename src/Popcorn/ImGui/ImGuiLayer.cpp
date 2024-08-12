#include "Popcorn/ImGui/ImGuiLayer.h"
#include "Event.h"
#include "Global_Macros.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

ENGINE_NAMESPACE_BEGIN
ImGuiLayer::ImGuiLayer() {}
ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::OnAttach() { ImGui::CreateContext(); }

void ImGuiLayer::OnDetach() {}

void ImGuiLayer::OnUpdate() {}

void ImGuiLayer::OnEvent(Event &e) {}
ENGINE_NAMESPACE_END
