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

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

# -----------------------------------------------------------------------
# BUILD INFO
# -----------------------------------------------------------------------
if(WIN32)
  list(APPEND CMAKE_PREFIX_PATH ../../install/windows) # to find Popcorn lib
elseif(APPLE)

elseif(UNIX)
  list(APPEND CMAKE_PREFIX_PATH ../../install/linux) # to find Popcorn lib
endif()
find_package(Popcorn REQUIRED)

add_executable(Game main.cpp)
target_link_libraries(Game PRIVATE Popcorn::Popcorn)

# -----------------------------------------------------------------------
# INSTALL INFO
# -----------------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS Game)
