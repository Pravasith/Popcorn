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
if(BUILD_FOR_LINUX)
  add_subdirectory("third-party/linux")
endif()

if(BUILD_FOR_WIN)
  add_subdirectory("third-party/windows")
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
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

# INCLUDE DIRS FOR LINUX
include_directories(# POPCORN INTERNAL INCLUDES
                    include)

if(BUILD_FOR_LINUX)
  # LINUX ENV BINARIES LOCATION
  set(LINUX_BINARIES_DIR ${CMAKE_SOURCE_DIR}/third-party/linux)

  # SET CMAKE_FIND ROOT PATHS TO FIND BINARIES OR COMPILERS
  set(CMAKE_FIND_ROOT_PATH
      /usr/x86_64-w64-mingw32 ${LINUX_BINARIES_DIR}/glfw
      ${LINUX_BINARIES_DIR}/glad ${LINUX_BINARIES_DIR}/imgui)

  # SPECIFY THE VARIABLE TO STORE THE HEADER FILE PATH
  set(GLAD_INCLUDE_DIR ${LINUX_BINARIES_DIR}/glad/include)
  set(GLFW_INCLUDE_DIR ${LINUX_BINARIES_DIR}/glfw/include)
  set(IMGUI_INCLUDE_DIR ${LINUX_BINARIES_DIR}/imgui/include)

  # FOR OTHER PLATFORMS USE TOOLCHAIN FILES
  include_directories(
    # THIRD PARTY INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/linux
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/linux/glad
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/linux/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/linux/glfw
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/linux/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/linux/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/linux/imgui/include)

  # USE FIND_PATH TO LOCATE THE HEADER FILE PATH
  find_path(GLAD_INCLUDE_DIR glad.h PATHS ${LINUX_BINARIES_DIR}/glad/include)
  find_path(GLFW_INCLUDE_DIR glfw3.h PATHS ${LINUX_BINARIES_DIR}/glfw/include)
  find_path(IMGUI_INCLUDE_DIR imgui.h PATHS ${LINUX_BINARIES_DIR}/imgui/include)

  # CHECK IF THE HEADER FILE PATH IS FOUND
  if(GLAD_INCLUDE_DIR
     AND GLFW_INCLUDE_DIR
     AND IMGUI_INCLUDE_DIR)
    message(STATUS "Found GLAD header files: ${GLAD_INCLUDE_DIR}")
    message(STATUS "Found GLFW header files: ${GLFW_INCLUDE_DIR}")
  else()
    message(FATAL_ERROR "Could not find GLAD header files")
    message(FATAL_ERROR "Could not find GLFW header files")
    message(FATAL_ERROR "Could not find ImGui header files")
  endif()
endif()

# TODO: Refactor these
target_include_directories(
  Window
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Core
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform)

target_include_directories(
  Events
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Input
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Events)

target_include_directories(
  PopcornEngine
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Events
         ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Core)

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
endif()
# -----------------------------------------------------------------------
# DEBUG FLAG --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# LINKING LIBS --- START
# -----------------------------------------------------------------------
target_link_libraries(Window PRIVATE glad glfw)
target_link_libraries(Window PUBLIC Events)
target_link_libraries(PopcornEngine PRIVATE Window Events)

# FINAL PROJECT LINKING
target_link_libraries(${PROJECT} PRIVATE PopcornEngine)
# -----------------------------------------------------------------------
# LINKING LIBS --- END
# -----------------------------------------------------------------------

# target_compile_features(${PROJECT} PUBLIC cxx_std_17)
