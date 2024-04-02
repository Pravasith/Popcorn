set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# where is the target environment located
set(WINDOWS_BINARIES_DIR ${CMAKE_SOURCE_DIR}/win64)
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32 ${WINDOWS_BINARIES_DIR}/glfw
                         ${WINDOWS_BINARIES_DIR}/glad)

# Specify the variable to store the header file path
set(GLAD_INCLUDE_DIR ${WINDOWS_BINARIES_DIR}/glad/include)
include_directories(${GLAD_INCLUDE_DIR})

# Use find_path to locate the header file path
find_path(GLAD_INCLUDE_DIR glad.h PATHS ${WINDOWS_BINARIES_DIR}/glad/include)

# Check if the header file path is found
if(GLAD_INCLUDE_DIR)
  message(STATUS "Found ${GLAD} header files: ${GLAD_INCLUDE_DIR}")
else()
  message(FATAL_ERROR "Could not find ${GLAD} header files")
endif()

# adjust the default behavior of the FIND_XXX() commands: search programs in the
# host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
