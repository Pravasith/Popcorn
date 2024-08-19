#include "Popcorn/ImGui/ImGuiLayer.h"
#include "Application.h"
#include "Event.h"
#include "GLFW_Funcs.h"
#include "Global_Macros.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <iostream>

ENGINE_NAMESPACE_BEGIN
ImGuiLayer::ImGuiLayer() {
  PC_PRINT_DEBUG("IMGUI LAYER OBJ CREATED", 2, "IMGUI_LAYER");
}
ImGuiLayer::~ImGuiLayer() {
  PC_PRINT_DEBUG("IMGUI LAYER OBJ DESTROYED", 2, "IMGUI_LAYER");
}

void ImGuiLayer::OnAttach() {
  Application &app = Application::Get();
  m_os_window = app.GetAppWindow().GetOSWindow();

  PC_PRINT_DEBUG("IMGUI LAYER INIT", 3, "IMGUI_LAYER");

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  // const char *glsl_version = "#version 100";
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // // GL 3.2 + GLSL 150
  // const char *glsl_version = "#version 150";
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
                                         //
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();
  //
  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
      static_cast<GLFW_Types::GLFW_OSWindow_T *>(m_os_window), true);
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Our state
  static bool show_demo_window = true;
  static bool show_another_window = false;
}

void ImGuiLayer::OnDetach() {}

void ImGuiLayer::OnUpdate() {
  // std::cout << "IMGUI UPDATE\n";
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // if (this.m_show_demo_window)
  auto show_demo_window = true;
  ImGui::ShowDemoWindow(&show_demo_window);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::OnEvent(Event &e) {
  std::cout << e.GetEventTypeName() << '\n';
}
ENGINE_NAMESPACE_END
