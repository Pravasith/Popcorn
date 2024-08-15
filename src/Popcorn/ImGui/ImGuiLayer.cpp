#include "Popcorn/ImGui/ImGuiLayer.h"
#include "Event.h"
#include "Global_Macros.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <iostream>

ENGINE_NAMESPACE_BEGIN
ImGuiLayer::ImGuiLayer() { std::cout << "CONSTRUCT\n"; }
ImGuiLayer::~ImGuiLayer() { std::cout << "DESTRUCT\n"; }

void ImGuiLayer::OnAttach() {}

void ImGuiLayer::OnDetach() {}

void ImGuiLayer::OnUpdate() {}

void ImGuiLayer::OnEvent(Event &e) {}
ENGINE_NAMESPACE_END
