cmake_minimum_required(VERSION 3.22)

set(PROJECT Triangle)
project(${PROJECT} VERSION 1.0.0)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# SPECIFY THE C++ STANDARD
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
option(STATIC_BUILD "BUILD A STATIC BINARY" ${BUILD_FOR_WIN})

if(STATIC_BUILD)
  set(CMAKE_EXE_LINKER_FLAGS "-static")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
  set(BUILD_SHARED_LIBS OFF)
endif()
# -----------------------------------------------------------------------
# STATIC BUILD SETTINGS (OPTIONAL) --- END
# -----------------------------------------------------------------------

#

set(PC_HOME "${CMAKE_SOURCE_DIR}/../../Engine")
set(CMAKE_PREFIX_PATH "${PC_HOME}/dist/linux")

# -----------------------------------------------------------------------
# ADD THIRD PARTY LIBS --- START
# -----------------------------------------------------------------------
# VENDOR BINARIES DIRS
set(PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR
    ${PC_HOME}/third-party/platform-agnostic)
set(LINUX_VENDOR_BINARIES_DIR ${PC_HOME}/third-party/linux)
set(WINDOWS_VENDOR_BINARIES_DIR ${PC_HOME}/third-party/windows)

if(BUILD_FOR_LINUX)
  # VENDOR BINARIES PATHS
  set(CMAKE_PREFIX_PATH
      # LINUX SPECIFIC
      ${LINUX_VENDOR_BINARIES_DIR}/glfw
      ${LINUX_VENDOR_BINARIES_DIR}/vulkan-loader
      # PLATFORM AGNOSTIC
      ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/vulkan-headers
      ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/glm)
else()
  set(CMAKE_PREFIX_PATH ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/vulkan-headers
                        ${PLATFORM_AGNOSTIC_VENDOR_BINARIES_DIR}/glm)
endif()

# FIND VENDOR BINARIES
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(VulkanLoader REQUIRED)
find_package(VulkanHeaders REQUIRED CONFIG)
find_package(glm CONFIG REQUIRED)
# -----------------------------------------------------------------------
# ADD THIRD PARTY LIBS --- END
# -----------------------------------------------------------------------

find_package(Popcorn REQUIRED)

# -----------------------------------------------------------------------
# SET EXECUTABLE --- START
# -----------------------------------------------------------------------
set(SOURCE_FILES main.cpp)
add_executable(${PROJECT} ${SOURCE_FILES})
# -----------------------------------------------------------------------
# SET EXECUTABLE --- END
# -----------------------------------------------------------------------

target_link_libraries(${PROJECT} PUBLIC Popcorn::Popcorn)

install(
  TARGETS Popcorn Triangle
  EXPORT TriangleTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# EXPORT THE INCLUDE DIRECTORIES
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

# # EXPORT THE GAME LIBRARY CONFIGURATION

# install( EXPORT PopcornTargets FILE PopcornConfig.cmake NAMESPACE Popcorn::
# DESTINATION lib/cmake/Popcorn)
