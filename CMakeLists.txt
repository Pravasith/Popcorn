cmake_minimum_required(VERSION 3.22)
# -----------------------------------------------------------------------
# Read - This is main project's CML, builds and installs different projects like
# Engine, and Editor
#
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# PROJECT INFO
# -----------------------------------------------------------------------
project(
  Popcorn
  VERSION 1.0.0
  LANGUAGES CXX)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# -----------------------------------------------------------------------
# OPTIONS & STUFF
# -----------------------------------------------------------------------
set(PC_ENGINE_DIR ${CMAKE_SOURCE_DIR}/Engine)
set(PC_EDITOR_DIR ${CMAKE_SOURCE_DIR}/Editor)
set(PC_ENGINE_INCLUDE_DIR ${PC_ENGINE_DIR}/include)

option(PC_DEBUG "USE DEBUG MODE" ON)
option(PC_PROFILING "USE PROFILING MODE" OFF)

# -----------------------------------------------------------------------
# BUILD & COMPILE STUFF
# -----------------------------------------------------------------------

if(WIN32)
  add_compile_definitions(PC_OS_WINDOWS)
elseif(APPLE)
  add_compile_definitions(PC_OS_MAC)
elseif(UNIX)
  add_compile_definitions(PC_OS_LINUX)
endif()

add_compile_definitions($<$<CONFIG:Debug>:PC_DEBUG>)
add_compile_definitions($<$<BOOL:${PC_PROFILING}>:PC_PROFILING>)

# No shared libs, we do static in here
set(BUILD_SHARED_LIBS OFF)

if(PC_PROFILING)
  # Link profilers
endif()

add_subdirectory(./third-party)
# add_subdirectory($PC_EDITOR_DIR/src)
add_subdirectory(${PC_ENGINE_DIR})

list(APPEND PC_LIBS_LIST glfw Vulkan::Vulkan)

# -----------------------------------------------------------------------
# INSTALLATION STUFF
# -----------------------------------------------------------------------

install(
  TARGETS ${PROJECT_NAME} Core Graphics glfw
  EXPORT PopcornTargets
  FILE_SET HEADERS)

include(GNUInstallDirs)

install(
  EXPORT PopcornTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Popcorn
  NAMESPACE Popcorn::)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/PopcornConfigVersion.cmake
  COMPATIBILITY SameMajorVersion)

install(FILES cmake/PopcornConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/PopcornConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Popcorn)
