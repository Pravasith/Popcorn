cmake_minimum_required(VERSION 3.22)

# -----------------------------------------------------------------------
# BASIC SETTINGS --- START
# -----------------------------------------------------------------------

set(PROJECT Popcorn)
project(${PROJECT} VERSION 1.0.0)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# SPECIFY THE C++ STANDARD
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

#

# -----------------------------------------------------------------------
# ADD THIRD PARTY LIBS --- START
# -----------------------------------------------------------------------
add_subdirectory("third-party")

# VENDOR BINARIES DIRS
set(PLATFORM_AGNOSTIC_VENDOR_DIR
    ${CMAKE_SOURCE_DIR}/third-party/platform-agnostic)
set(LINUX_VENDOR_DIR ${CMAKE_SOURCE_DIR}/third-party/linux)
set(WINDOWS_VENDOR_DIR ${CMAKE_SOURCE_DIR}/third-party/windows)

# Set CMAKE_PREFIX_PATH for find_package
if(BUILD_FOR_LINUX)
  list(APPEND CMAKE_PREFIX_PATH ${LINUX_VENDOR_DIR}/glfw
       ${LINUX_VENDOR_DIR}/vulkan-loader)
endif()
list(APPEND CMAKE_PREFIX_PATH ${PLATFORM_AGNOSTIC_VENDOR_DIR}/vulkan-headers
     ${PLATFORM_AGNOSTIC_VENDOR_DIR}/glm)

# FIND VENDOR BINARIES
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(VulkanLoader REQUIRED)
find_package(VulkanHeaders REQUIRED CONFIG)
find_package(glm CONFIG REQUIRED)
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

#

# -----------------------------------------------------------------------
# SET IMPLEMENTATION AND INCLUDE DIRS --- START
# -----------------------------------------------------------------------
add_subdirectory("src")

include_directories(include)

target_include_directories(
  glad
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third-party/platform-agnostic/glad/include>
    $<INSTALL_INTERFACE:include>)

target_include_directories(
  ImGui
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third-party/platform-agnostic>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third-party/platform-agnostic/imgui>
    $<INSTALL_INTERFACE:include>)

# PRECOMPILE
target_precompile_headers(
  glad PUBLIC ${PLATFORM_AGNOSTIC_VENDOR_DIR}/glad/include/glad/glad.h)

target_include_directories(
  Window
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/Agnostic>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include/Popcorn/Core>
    $<INSTALL_INTERFACE:include/Popcorn/Platform>
    $<INSTALL_INTERFACE:include/Popcorn/Platform/Agnostic>)

target_include_directories(
  Events
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Input>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Events>
         $<INSTALL_INTERFACE:include>
         $<INSTALL_INTERFACE:include/Popcorn/Input>
         $<INSTALL_INTERFACE:include/Popcorn/Events>)

target_include_directories(
  Graphics
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Graphics>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/OpenGL>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/Vulkan>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include/Popcorn/Graphics>
    $<INSTALL_INTERFACE:include/Popcorn/Platform/OpenGL>
    $<INSTALL_INTERFACE:include/Popcorn/Platform/Vulkan>)

target_include_directories(
  Popcorn
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Core>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Layers>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Events>
         $<INSTALL_INTERFACE:include>
         $<INSTALL_INTERFACE:include/Popcorn/Core>
         $<INSTALL_INTERFACE:include/Popcorn/Layers>
         $<INSTALL_INTERFACE:include/Popcorn/Events>)

# -----------------------------------------------------------------------
# SET IMPLEMENTATION AND INCLUDE DIRS --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# DEBUG FLAG --- START
# -----------------------------------------------------------------------
option(PC_DEBUG "USE DEBUG MODE FOR COMPILING" ON)
# option(PC_DEBUG "USE DEBUG MODE FOR COMPILING" OFF)

if(PC_DEBUG)
  target_compile_definitions(Window PUBLIC "PC_DEBUG")
  target_compile_definitions(Graphics PUBLIC "PC_DEBUG")
  target_compile_definitions(Events PUBLIC "PC_DEBUG")
endif()
# -----------------------------------------------------------------------
# DEBUG FLAG --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# LINKING LIBS --- START
# -----------------------------------------------------------------------
target_link_libraries(Graphics PRIVATE Vulkan::Headers Vulkan::Loader glfw
                                       glm::glm)
target_link_libraries(Window PRIVATE glad glfw)
target_link_libraries(Window PUBLIC Events)
target_link_libraries(ImGui PUBLIC glfw)
target_link_libraries(Popcorn PRIVATE glad)
target_link_libraries(Popcorn PUBLIC ImGui)

target_link_libraries(Popcorn PRIVATE Graphics Window)
# -----------------------------------------------------------------------
# LINKING LIBS --- END
# -----------------------------------------------------------------------

# target_compile_features(${PROJECT} PUBLIC cxx_std_17)

install(
  TARGETS Popcorn Window Graphics ImGui glad Events
  EXPORT PopcornTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# Export the include directories
install(DIRECTORY include/ DESTINATION include)

# Export the Popcorn library configuration
install(
  EXPORT PopcornTargets
  FILE PopcornConfig.cmake
  NAMESPACE Popcorn::
  DESTINATION lib/cmake/Popcorn)
