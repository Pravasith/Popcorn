cmake_minimum_required(VERSION 3.22)

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

# -----------------------------------------------------------------------
# BUILD & COMPILE STUFF
# -----------------------------------------------------------------------
option(PC_DEBUG "USE PROFILING MODE" OFF)
option(PC_PROFILING "USE PROFILING MODE" OFF)
