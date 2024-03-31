cmake_minimum_required(VERSION 3.22)

set(PROJECT GOOD_STUFF)

project(${PROJECT} VERSION 1.0.0)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(WINDOWS_BINARIES_DIR ${CMAKE_SOURCE_DIR}/win64)
set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/win64/glad/include)
include_directories(${GLAD_INCLUDE_DIR})

set(GLFW_BUILD_DOCS
    OFF
    CACHE BOOL "" FORCE)

set(GLFW_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)

set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)

set(SOURCE_FILES main.cpp)

# Include libraries like GLAD
add_subdirectory("third-party")

add_executable(${PROJECT} ${SOURCE_FILES})

set(glfw3_DIR ${WINDOWS_BINARIES_DIR}/glfw/lib/cmake/glfw3)
find_package(glfw3 REQUIRED)

# Include GLAD headers for the project
target_include_directories(${PROJECT} PUBLIC ${GLAD_INCLUDE_DIR})

target_link_libraries(${PROJECT} PUBLIC glad glfw)

# target_compile_features(${PROJECT} PUBLIC cxx_std_17)
