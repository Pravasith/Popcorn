set(CORE_IMPL_DIR Popcorn/Core)
set(IMGUI_IMPL_DIR Popcorn/ImGui)
set(EVENTS_IMPL_DIR Popcorn/Events)
set(PLATFORM_IMPL_DIR Popcorn/Platform)
set(GRAPHICS_IMPL_DIR Popcorn/Graphics)

add_library(
  PopcornEngine
  ${CORE_IMPL_DIR}/Application.cpp ${CORE_IMPL_DIR}/Layer.cpp
  ${CORE_IMPL_DIR}/LayerStack.cpp ${IMGUI_IMPL_DIR}/ImGuiLayer.cpp)

add_library(Window ${CORE_IMPL_DIR}/Window.cpp
                   ${PLATFORM_IMPL_DIR}/Agnostic/Window_Agnostic.cpp)

add_library(Events ${EVENTS_IMPL_DIR}/Publisher.cpp)
add_library(
  Graphics
  ${GRAPHICS_IMPL_DIR}/Renderer.cpp
  ${PLATFORM_IMPL_DIR}/OpenGL/Renderer_OpenGL.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/Renderer_Vulkan.cpp)
