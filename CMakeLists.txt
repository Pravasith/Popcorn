cmake_minimum_required(VERSION 3.22)

set(PROJECT Game)

project(${PROJECT} VERSION 1.0.0)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# STATIC stuff (Windows)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(BUILD_FOR_WIN TRUE)
  # Include the toolchain file for cross-compilation
  # include(${CMAKE_TOOLCHAIN_FILE})
endif()

option(STATIC_BUILD "Build a static binary." ${BUILD_FOR_WIN})

if(STATIC_BUILD)
  set(CMAKE_EXE_LINKER_FLAGS "-static")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
  set(BUILD_SHARED_LIBS OFF)

  # set(WINDOWS_BINARIES_DIR ${CMAKE_SOURCE_DIR}/win64) set(GLAD_INCLUDE_DIR
  # ${WINDOWS_BINARIES_DIR}/glad/include)
endif()

set(SOURCE_FILES main.cpp)

# Third party libraries
add_subdirectory("third-party")

add_executable(${PROJECT} ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# GLFW build settings
set(GLFW_BUILD_DOCS
    OFF
    CACHE BOOL "" FORCE)

set(GLFW_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)

set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)

# Implimentation dirs
add_subdirectory("src")

# Include dirs
include_directories(include)

target_include_directories(
  Window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/core)

target_include_directories(
  PopcornEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                       ${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/core)

target_link_libraries(Window PUBLIC glad glfw)
target_link_libraries(PopcornEngine PRIVATE Window)

# FINAL PROJECT LINKING
target_link_libraries(${PROJECT} PRIVATE PopcornEngine)

# target_compile_features(${PROJECT} PUBLIC cxx_std_17)
