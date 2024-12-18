set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# WINDOWS ENV BINARIES LOCATION
set(PC_HOME "${CMAKE_SOURCE_DIR}/../../Engine")

# SET CMAKE_FIND ROOT PATHS TO FIND BINARIES OR COMPILERS
set(CMAKE_FIND_ROOT_PATH
    # COMPILER
    /usr/x86_64-w64-mingw32
    #
    # POPCORN ENGINE
    ${PC_HOME}/dist/windows
    #
    "${PC_HOME}/dist/windows/third-party/glfw"
    "${PC_HOME}/dist/windows/third-party/vulkan-loader"
    "${PC_HOME}/dist/windows/third-party/vulkan-headers"
    "${PC_HOME}/dist/windows/third-party/glm")

# SPECIFY THE VARIABLE TO STORE THE HEADER FILE PATH

set(GLFW_INCLUDE_DIR
    ${WINDOWS_BINARIES_DIR}/glfw/include ${WINDOWS_BINARIES_DIR}/vulkan-loader
    ${WINDOWS_BINARIES_DIR}/vulkan-loader/lib)

include_directories(${GLFW_INCLUDE_DIR})

# USE FIND_PATH TO LOCATE THE HEADER FILE PATH

find_path(GLFW_INCLUDE_DIR glfw3.h PATHS ${WINDOWS_BINARIES_DIR}/glfw/include)

# CHECK IF THE HEADER FILE PATH IS FOUND

if(GLFW_INCLUDE_DIR)
  message(STATUS "Found GLFW header files:
${GLFW_INCLUDE_DIR}")
else()
  message(FATAL_ERROR "Could not find GLFW header
files")
endif()

# adjust the default behavior of the FIND_XXX() commands: search programs in the
# host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
