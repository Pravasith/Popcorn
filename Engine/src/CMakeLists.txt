set(CORE_IMPL_DIR Popcorn/Core)
set(EVENTS_IMPL_DIR Popcorn/Events)
set(GRAPHICS_IMPL_DIR Popcorn/Graphics)
set(LAYERS_IMPL_DIR Popcorn/Layers)
set(LOADERS_IMPL_DIR Popcorn/Loaders)
set(PLATFORM_IMPL_DIR Popcorn/Platform)
set(SCENE_IMPL_DIR Popcorn/Scene)

# POPCORN LIB
add_library(
  Popcorn
  #
  # CORE
  ${CORE_IMPL_DIR}/Application.cpp
  ${CORE_IMPL_DIR}/Externs.cpp
  ${CORE_IMPL_DIR}/Layer.cpp
  ${CORE_IMPL_DIR}/LayerStack.cpp
  ${CORE_IMPL_DIR}/Time.cpp
  #
  # LOADERS
  ${LAYERS_IMPL_DIR}/DebugUIOverlay.cpp
  #
  # LOADERS
  ${LOADERS_IMPL_DIR}/GltfLoader.cpp)

# -----------------------------------------------------------------------
#

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
  ${GRAPHICS_IMPL_DIR}/BufferObjects.cpp
  ${GRAPHICS_IMPL_DIR}/Mesh.cpp
  ${GRAPHICS_IMPL_DIR}/Materials/Material.cpp
  ${GRAPHICS_IMPL_DIR}/Shader.cpp
  #
  # SCENE CLASSES
  # TODO: Separate the scene to a different library when time comes
  ${SCENE_IMPL_DIR}/Scene.cpp
  ${SCENE_IMPL_DIR}/GameObject.cpp
  #
  # OPENGL CLASSES
  ${PLATFORM_IMPL_DIR}/OpenGL/RendererOpenGL.cpp
  #
  # VULKAN CLASSES
  ${PLATFORM_IMPL_DIR}/Vulkan/RendererVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/DeviceVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/SurfaceVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/SwapchainVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/FramebuffersVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/BufferObjectsVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/FrameVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/CommandPoolVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/ImageVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/Descriptors/DescriptorsVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/Pipelines/GfxPipelineVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/Pipelines/PipelineVk.cpp
  ${PLATFORM_IMPL_DIR}/Vulkan/Memory/MemoryAllocatorVk.cpp
  #
  # RENDER WORK FLOWS
  ${PLATFORM_IMPL_DIR}/Vulkan/RenderFlows/BasicRenderFlowVk.cpp)

#
