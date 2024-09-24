#include <glad/glad.h>

#include "Application.h"
#include "Event.h"
#include "GLFW_Funcs.h"
#include "Global_Macros.h"
#include "Popcorn/ImGui/ImGuiLayer.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <iostream>

ENGINE_NAMESPACE_BEGIN
ImGuiLayer::ImGuiLayer() { PC_PRINT_DEBUG("CREATED", 1, "IMGUI-LAYER"); }
ImGuiLayer::~ImGuiLayer() { PC_PRINT_DEBUG("DESTROYED", 1, "IMGUI-LAYER"); }

void ImGuiLayer::OnAttach() {

  std::cout << "s_instance";
  Application &app = Application::Get();
  m_os_window = app.GetAppWindow().GetOSWindow();

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
  m_imgui_io = ImGui::GetIO();
  (void)m_imgui_io;

  m_imgui_io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
                                          //
  // io.ConfigFlags |=
  //     ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
  //
  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  ImGui::StyleColorsLight();

  m_imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  m_imgui_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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

  if (m_imgui_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow *backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }

  static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  int display_w, display_h;
  glfwGetFramebufferSize(
      static_cast<GLFW_Types::GLFW_OSWindow_T *>(m_os_window), &display_w,
      &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
               clear_color.z * clear_color.w, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::OnEvent(Event &e) {
  // TODO: ADD EVENTS
  std::cout << e.GetEventTypeName() << '\n';
}
ENGINE_NAMESPACE_END
