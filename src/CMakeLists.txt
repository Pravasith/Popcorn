# CORE LIBS
set(CORE_IMPL_DIR Popcorn/Core)

add_library(
  PopcornEngine
  #
  # CORE IMPL FILES
  ${CORE_IMPL_DIR}/Application.cpp
  # CORE LAYERS
  ${CORE_IMPL_DIR}/Layers/Layer.cpp
  ${CORE_IMPL_DIR}/Layers/LayerStack.cpp
  ${CORE_IMPL_DIR}/Layers/DebugUIOverlay.cpp
  # TIME
  ${CORE_IMPL_DIR}/Time.cpp)

# -----------------------------------------------------------------------
# SEPARATE LIBS
set(EVENTS_IMPL_DIR Popcorn/Events)
set(PLATFORM_IMPL_DIR Popcorn/Platform)
set(GRAPHICS_IMPL_DIR Popcorn/Graphics)

add_library(Window ${CORE_IMPL_DIR}/Window.cpp
                   ${PLATFORM_IMPL_DIR}/Agnostic/WindowAgnostic.cpp)

add_library(Events ${EVENTS_IMPL_DIR}/Publisher.cpp)
add_library(
  Graphics
  ${GRAPHICS_IMPL_DIR}/Renderer.cpp
  ${PLATFORM_IMPL_DIR}/OpenGL/RendererOpenGL.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/RendererVulkan.cpp)
