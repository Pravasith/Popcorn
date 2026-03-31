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

find_package(Popcorn REQUIRED)

# Your project setup here

target_link_libraries(Game PRIVATE Popcorn)

# -----------------------------------------------------------------------
# BUILD INFO
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# INSTALL INFO
# -----------------------------------------------------------------------
