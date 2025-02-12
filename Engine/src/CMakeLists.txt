set(CORE_IMPL_DIR Popcorn/Core)
set(LAYERS_IMPL_DIR Popcorn/Layers)

# POPCORN LIB
add_library(
  Popcorn
  #
  # CORE IMPL FILES
  ${CORE_IMPL_DIR}/Application.cpp
  ${CORE_IMPL_DIR}/Externs.cpp
  #
  # CORE LAYERS
  ${CORE_IMPL_DIR}/Layer.cpp
  ${CORE_IMPL_DIR}/LayerStack.cpp
  ${LAYERS_IMPL_DIR}/DebugUIOverlay.cpp
  #
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
  #
  # MAIN CLASS
  ${GRAPHICS_IMPL_DIR}/Renderer.cpp
  ${GRAPHICS_IMPL_DIR}/VertexBuffer.cpp
  #
  # OPENGL CLASSES
  ${PLATFORM_IMPL_DIR}/OpenGL/RendererOpenGL.cpp
  #
  # VULKAN CLASSES
  ${PLATFORM_IMPL_DIR}/Vulkan/RendererVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/DeviceVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/SurfaceVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/SwapchainVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/VertexBufferVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/Pipelines/GfxPipelineVk.cpp)
