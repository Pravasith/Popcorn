cmake_minimum_required(VERSION 3.22)

# -----------------------------------------------------------------------
# PROJECT INFO
# -----------------------------------------------------------------------
project(
  ExampleGame
  VERSION 1.0.0
  LANGUAGES CXX)

message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(
  APPEND
  CMAKE_PREFIX_PATH
  #
  # vulkan loader binaries
  #
  # ../../build/linux/third-party
  #
  # popcorn lib
  ../../install/linux)

# needs vulkan loader
find_package(VulkanLoader REQUIRED)
find_package(Popcorn REQUIRED)

add_executable(Game main.cpp)
target_link_libraries(Game PRIVATE Popcorn::Popcorn)

# -----------------------------------------------------------------------
# BUILD INFO
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# INSTALL INFO
# -----------------------------------------------------------------------
