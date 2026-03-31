cmake_minimum_required(VERSION 3.22)
# -----------------------------------------------------------------------
# Read - This is main project's CML, builds and installs different projects like
# -
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# PROJECT INFO
# -----------------------------------------------------------------------
project(
  Popcorn
  VERSION 1.0.0
  LANGUAGES CXX)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# make cache variables for install destinations
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------
# BUILD & COMPILE STUFF
# -----------------------------------------------------------------------
option(PC_DEBUG "USE PROFILING MODE" OFF)
option(PC_PROFILING "USE PROFILING MODE" OFF)

if(WIN32)
  add_compile_definitions(PC_OS_WINDOWS)
elseif(APPLE)
  add_compile_definitions(PC_OS_MAC)
elseif(UNIX)
  add_compile_definitions(PC_OS_LINUX)
endif()

add_compile_definitions($<$<CONFIG:Debug>:PC_DEBUG>)
add_compile_definitions($<$<BOOL:${PC_PROFILING}>:PC_PROFILING>)

set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(POPCORN_INCLUDE_DIR ${PROJECT_INCLUDE_DIR}/Popcorn)
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third-party)

# No shared libs, we do static in here
set(BUILD_SHARED_LIBS OFF)

# -----------------------------------------------------------------------
# DEPENDENCIES
# -----------------------------------------------------------------------
# find_package(OpenGL REQUIRED) find_package(glfw3 REQUIRED)
# find_package(VulkanLoader REQUIRED) find_package(VulkanHeaders REQUIRED
# CONFIG) find_package(glm CONFIG REQUIRED)
