set(PLATFORM_AGNOSTIC_DIR platform-agnostic)

add_library(glad ${PLATFORM_AGNOSTIC_DIR}/glad/src/glad.c)
add_library(
  ImGui
  # COMMON
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui_demo.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui_draw.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui_tables.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui_widgets.cpp
  # BACKENDS
  ${PLATFORM_AGNOSTIC_DIR}/imgui/backends/imgui_impl_opengl3.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/backends/imgui_impl_glfw.cpp)
