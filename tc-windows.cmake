set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# WINDOWS ENV BINARIES LOCATION
set(WINDOWS_BINARIES_DIR ${CMAKE_SOURCE_DIR}/third-party/windows)

# SET CMAKE_FIND ROOT PATHS TO FIND BINARIES OR COMPILERS
set(CMAKE_FIND_ROOT_PATH
    /usr/x86_64-w64-mingw32 ${WINDOWS_BINARIES_DIR}/glfw
    ${WINDOWS_BINARIES_DIR}/glad ${WINDOWS_BINARIES_DIR}/imgui)

# SPECIFY THE VARIABLE TO STORE THE HEADER FILE PATH
set(GLAD_INCLUDE_DIR ${WINDOWS_BINARIES_DIR}/glad/include)
set(GLFW_INCLUDE_DIR ${WINDOWS_BINARIES_DIR}/glfw/include)
set(IMGUI_INCLUDE_DIR ${WINDOWS_BINARIES_DIR}/imgui/include)

include_directories(${GLAD_INCLUDE_DIR} ${GLFW_INCLUDE_DIR}
                    ${IMGUI_INCLUDE_DIR})

# USE FIND_PATH TO LOCATE THE HEADER FILE PATH
find_path(GLAD_INCLUDE_DIR glad.h PATHS ${WINDOWS_BINARIES_DIR}/glad/include)
find_path(GLFW_INCLUDE_DIR glfw3.h PATHS ${WINDOWS_BINARIES_DIR}/glfw/include)
find_path(IMGUI_INCLUDE_DIR imgui.h PATHS ${WINDOWS_BINARIES_DIR}/imgui/include)

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

# adjust the default behavior of the FIND_XXX() commands: search programs in the
# host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
