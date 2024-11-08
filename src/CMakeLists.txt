set(CORE_IMPL_DIR Popcorn/Core)
set(LAYERS_IMPL_DIR Popcorn/Layers)

# POPCORN LIB
add_library(
  PopcornEngine
  #
  # CORE IMPL FILES
  ${CORE_IMPL_DIR}/Application.cpp
  # CORE LAYERS
  ${CORE_IMPL_DIR}/Layer.cpp
  ${CORE_IMPL_DIR}/LayerStack.cpp
  ${LAYERS_IMPL_DIR}/DebugUIOverlay.cpp
  ${LAYERS_IMPL_DIR}/RenderLayer.cpp
  # TIME
  ${CORE_IMPL_DIR}/Time.cpp)

# -----------------------------------------------------------------------
#

set(EVENTS_IMPL_DIR Popcorn/Events)
set(PLATFORM_IMPL_DIR Popcorn/Platform)
set(GRAPHICS_IMPL_DIR Popcorn/Graphics)

# WINDOW LIB
add_library(Window ${CORE_IMPL_DIR}/Window.cpp
                   ${PLATFORM_IMPL_DIR}/Agnostic/WindowAgnostic.cpp)

# EVENTS LIB
add_library(Events ${EVENTS_IMPL_DIR}/Publisher.cpp)

# GRAPHICS LIB
add_library(
  Graphics
  ${GRAPHICS_IMPL_DIR}/Renderer.cpp
  ${PLATFORM_IMPL_DIR}/OpenGL/RendererOpenGL.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/RendererVulkan.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/VkValidationLayers.cpp)
