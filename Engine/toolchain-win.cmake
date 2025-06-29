set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# set(LINUX_BINARIES_DIR ${CMAKE_SOURCE_DIR}/third-party/linux)
set(WINDOWS_BINARIES_DIR ${CMAKE_SOURCE_DIR}/third-party/windows)
set(PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR
    ${CMAKE_SOURCE_DIR}/third-party/platform-agnostic)

# SET CMAKE_FIND ROOT PATHS TO FIND BINARIES OR COMPILERS
set(CMAKE_FIND_ROOT_PATH
    # COMPILER
    /usr/x86_64-w64-mingw32
    #
    # VENDOR LIBS -- WIN SPECIFIC
    ${WINDOWS_BINARIES_DIR}/glfw
    ${WINDOWS_BINARIES_DIR}/vulkan-loader
    ${WINDOWS_BINARIES_DIR}/vulkan-loader/bin
    #
    # VENDOR LIBS -- PL AGNOSTIC
    ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/vma
    ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/vulkan-headers
    ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/tiny-gltf
    ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/glm)

# adjust the default behavior of the FIND_XXX() commands: search programs in the
# host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
