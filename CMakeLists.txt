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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
add_subdirectory($PC_ENGINE_DIR/src)

find_package(Vulkan REQUIRED)

list(APPEND PC_LIBS_LIST glfw Vulkan::Vulkan)

# -----------------------------------------------------------------------
# LINKING STUFF
# -----------------------------------------------------------------------
target_link_libraries(Graphics PRIVATE Vulkan::Vulkan)
target_link_libraries(Core PRIVATE $PC_LIBS_LIST)
target_link_libraries($PROJECT_NAME PUBLIC Core Graphics)

# -----------------------------------------------------------------------
# INSTALLATION STUFF - WE DO PORTABLE IN HERE
# -----------------------------------------------------------------------

# Read this section for cmake tuts to understand why i'm setting these -
# https://cmake.org/cmake/help/latest/guide/tutorial/Installation%20Commands%20and%20Concepts.html#background
set(PC_INSTALL_DIR ${CMAKE_SOURCE_DIR}/install/)
set(PC_INSTALL_BINDIR ${PC_INSTALL_DIR}/bin) # For binaries (RUNTIME)
set(PC_INSTALL_LIBDIR ${PC_INSTALL_DIR}/lib) # For libs (LIBRARY and ARCHIVE)
set(PC_INSTALL_INCLUDEDIR ${PC_INSTALL_DIR}/include) # For includes
# (PRIVATE_HEADER, PUBLIC HEADER, FILE_SET)

# include(GNUInstallDirs)
install(
  TARGETS $PROJECT_NAME Core Graphics FILE_SET HEADERS
  DESTINATION RUNTIME
  $PC_INSTALL_BINDIR
  DESTINATION LIBRARY
  $PC_INSTALL_LIBDIR
  DESTINATION ARCHIVE
  $PC_INSTALL_LIBDIR
  DESTINATION PRIVATE_HEADER
  $PC_INSTALL_INCLUDEDIR
  DESTINATION PUBLIC_HEADER
  $PC_INSTALL_INCLUDEDIR
  DESTINATION FILE_SET
  $PC_INSTALL_INCLUDEDIR
  EXPORT PopcornTargets)
install(
  TARGETS PopcornTargets
  DESTINATION ${PC_INSTALL_LIBDIR}/cmake/Popcorn
  NAMESPACE Popcorn::)

include(${CMAKE_CURRENT_LIST_DIR}/PopcornTargets.cmake)

include(CMakePackageConfigHelpers)
write_basic_package_config_file(
  ${CMAKE_CURRENT_BINARY_DIR}/PopcornConfigVersion.cmake COMPATIBILITY
  SameMajorVersion)

install(FILES cmake/PopcornConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/PopcornConfigVersion.cmake
        DESTINATION ${PC_INSTALL_LIBDIR}/cmake/Popcorn)
