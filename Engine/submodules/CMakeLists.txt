set(PLATFORM_AGNOSTIC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/platform-agnostic)

# GLFW
set(GLFW_DIR ./glfw) # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
include_directories(${GLFW_DIR}/include)

# Dear ImGui
set(IMGUI_DIR ${PLATFORM_AGNOSTIC_DIR}/imgui)
add_library(
  ImGui
  # ImGui
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp)

add_library(vma ${PLATFORM_AGNOSTIC_DIR}/vma/vk_mem_alloc.cpp)
