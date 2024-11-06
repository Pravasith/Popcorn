#

# -----------------------------------------------------------------------
# BASIC SETTINGS --- START
# -----------------------------------------------------------------------
cmake_minimum_required(VERSION 3.22)

set(PROJECT Game)

project(${PROJECT} VERSION 1.0.0)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# -----------------------------------------------------------------------
# BASIC SETTINGS --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# OS FLAGS --- START
# -----------------------------------------------------------------------
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(BUILD_FOR_LINUX TRUE)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(BUILD_FOR_WIN TRUE)
  # Include the toolchain file for cross-compilation
  # include(${CMAKE_TOOLCHAIN_FILE})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(BUILD_FOR_MACOS TRUE)
  # Include the toolchain file for cross-compilation
  # include(${CMAKE_TOOLCHAIN_FILE})
endif()
# -----------------------------------------------------------------------
# OS FLAGS --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# STATIC BUILD SETTINGS (OPTIONAL) --- START
# -----------------------------------------------------------------------
option(STATIC_BUILD "Build a static binary." ${BUILD_FOR_WIN})

if(STATIC_BUILD)
  set(CMAKE_EXE_LINKER_FLAGS "-static")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
  set(BUILD_SHARED_LIBS OFF)

  # set(WINDOWS_BINARIES_DIR ${CMAKE_SOURCE_DIR}/win64) set(GLAD_INCLUDE_DIR
  # ${WINDOWS_BINARIES_DIR}/glad/include)
endif()
# -----------------------------------------------------------------------
# STATIC BUILD SETTINGS (OPTIONAL) --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# SET EXECUTABLE --- START
# -----------------------------------------------------------------------
set(SOURCE_FILES main.cpp)
add_executable(${PROJECT} ${SOURCE_FILES})
# -----------------------------------------------------------------------
# SET EXECUTABLE --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# ADD THIRD PARTY LIBS --- START
# -----------------------------------------------------------------------
# if(BUILD_FOR_LINUX) add_subdirectory("third-party/linux") endif()
#
# if(BUILD_FOR_WIN) add_subdirectory("third-party/windows") endif()

add_subdirectory("third-party")

# VENDOR BINARIES DIRS
set(PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR
    ${CMAKE_SOURCE_DIR}/third-party/platform-agnostic)
set(LINUX_VENDOR_BINARIES_DIR ${CMAKE_SOURCE_DIR}/third-party/linux)
set(WINDOWS_VENDOR_BINARIES_DIR ${CMAKE_SOURCE_DIR}/third-party/windows)

if(BUILD_FOR_LINUX)
  # VENDOR BINARIES PATHS
  set(CMAKE_PREFIX_PATH
      # LINUX SPECIFIC
      ${LINUX_VENDOR_BINARIES_DIR}/glfw
      ${LINUX_VENDOR_BINARIES_DIR}/vulkan-loader
      # PLATFORM AGNOSTIC
      ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/vulkan-headers
      ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/glm)
else()
  set(CMAKE_PREFIX_PATH ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/vulkan-headers
                        ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/glm)
endif()

# FIND VENDOR BINARIES
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(VulkanLoader REQUIRED)
find_package(VulkanHeaders REQUIRED CONFIG)
find_package(glm CONFIG REQUIRED)
# -----------------------------------------------------------------------
# ADD THIRD PARTY LIBS --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# GLFW SETTINGS --- START
# -----------------------------------------------------------------------
set(GLFW_BUILD_DOCS
    OFF
    CACHE BOOL "" FORCE)

set(GLFW_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)

set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
# -----------------------------------------------------------------------
# GLFW SETTINGS --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# SET IMPLEMENTATION AND INCLUDE DIRS --- START
# -----------------------------------------------------------------------
# Main implementation dir
add_subdirectory("src")

include_directories(# POPCORN INTERNAL INCLUDES
                    include)

target_include_directories(
  glad PUBLIC ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/glad/include)
target_include_directories(
  ImGui PUBLIC ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}
               ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/imgui)

target_precompile_headers(
  glad PUBLIC ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/glad/include/glad/glad.h)

# TODO: Refactor these
target_include_directories(
  Window
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Core
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/Agnostic)

target_include_directories(
  Events
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Input
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Events)

target_include_directories(
  Graphics
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Graphics
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/OpenGL
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/Vulkan)

target_include_directories(
  PopcornEngine
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Core
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Layers
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Events)

# -----------------------------------------------------------------------
# SET IMPLEMENTATION AND INCLUDE DIRS --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# DEBUG FLAG --- START
# -----------------------------------------------------------------------
option(PC_DEBUG "Use debug mode for compiling" ON)
# option(PC_DEBUG "Use debug mode for compiling" OFF)

if(PC_DEBUG)
  target_compile_definitions(Window PUBLIC "PC_DEBUG")
  target_compile_definitions(Graphics PUBLIC "PC_DEBUG")
  target_compile_definitions(Events PUBLIC "PC_DEBUG")
endif()
# -----------------------------------------------------------------------
# DEBUG FLAG --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# LINKING LIBS --- START
# -----------------------------------------------------------------------
target_link_libraries(Graphics PRIVATE Vulkan::Headers Vulkan::Loader glfw)
target_link_libraries(Window PRIVATE glad glfw)
target_link_libraries(Window PUBLIC Events)

# FOR IMGUI_LAYER
target_link_libraries(PopcornEngine PRIVATE glad)
target_link_libraries(ImGui PUBLIC glfw)

target_link_libraries(PopcornEngine PUBLIC ImGui)
target_link_libraries(PopcornEngine PRIVATE Graphics Window)

# FINAL PROJECT LINKING
target_link_libraries(${PROJECT} PRIVATE PopcornEngine)
# -----------------------------------------------------------------------
# LINKING LIBS --- END
# -----------------------------------------------------------------------

# target_compile_features(${PROJECT} PUBLIC cxx_std_17)
