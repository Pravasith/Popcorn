set(PLATFORM_AGNOSTIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/platform-agnostic)

# add_library(glad ${PLATFORM_AGNOSTIC_DIR}/glad/src/glad.c)

add_library(
  ImGui
  # ImGui
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui_demo.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui_draw.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui_tables.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/imgui_widgets.cpp
  # BACKENDS
  ${PLATFORM_AGNOSTIC_DIR}/imgui/backends/imgui_impl_opengl3.cpp
  ${PLATFORM_AGNOSTIC_DIR}/imgui/backends/imgui_impl_glfw.cpp)

add_library(vma ${PLATFORM_AGNOSTIC_DIR}/vma/vk_mem_alloc.cpp)
