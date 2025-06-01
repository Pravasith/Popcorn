cmake_minimum_required(VERSION 3.22)

set(PROJECT Popcorn)
project(${PROJECT} VERSION 1.0.0)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
# OS FLAGS
# -----------------------------------------------------------------------
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(BUILD_FOR_LINUX TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(BUILD_FOR_WIN TRUE)
  # include(${CMAKE_TOOLCHAIN_FILE})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(BUILD_FOR_MACOS TRUE)
  # include(${CMAKE_TOOLCHAIN_FILE})
endif()

set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(POPCORN_INCLUDE_DIR ${PROJECT_INCLUDE_DIR}/Popcorn)
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third-party)

# -----------------------------------------------------------------------
# STATIC BUILD SETTINGS
# -----------------------------------------------------------------------
option(STATIC_BUILD "BUILD A STATIC BINARY" ${BUILD_FOR_WIN})
if(STATIC_BUILD)
  set(CMAKE_EXE_LINKER_FLAGS "-static")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
  set(BUILD_SHARED_LIBS OFF)
endif()

add_subdirectory("third-party")
add_subdirectory("src")

# -----------------------------------------------------------------------
# ADD THIRD-PARTY/VENDOR LIBS
# -----------------------------------------------------------------------
set(PLATFORM_AGNOSTIC_VENDOR_DIR
    ${CMAKE_SOURCE_DIR}/third-party/platform-agnostic)
set(LINUX_VENDOR_DIR ${CMAKE_SOURCE_DIR}/third-party/linux)
set(WINDOWS_VENDOR_DIR ${CMAKE_SOURCE_DIR}/third-party/windows)

# Set CMAKE_PREFIX_PATH for find_package
if(BUILD_FOR_LINUX)
  list(APPEND CMAKE_PREFIX_PATH ${LINUX_VENDOR_DIR}/glfw
       ${LINUX_VENDOR_DIR}/vulkan-loader)
elseif(BUILD_FOR_WIN)
  # Add from toolchain file
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
# TARGETS
# -----------------------------------------------------------------------

# GLAD
target_include_directories(
  glad
  PUBLIC $<BUILD_INTERFACE:${PLATFORM_AGNOSTIC_VENDOR_DIR}/glad/include>
  PRIVATE $<INSTALL_INTERFACE:third-party>)
target_precompile_headers(
  glad PRIVATE ${PLATFORM_AGNOSTIC_VENDOR_DIR}/glad/include/glad/glad.h)

# VMA
target_include_directories(
  vma PRIVATE $<BUILD_INTERFACE:${PLATFORM_AGNOSTIC_VENDOR_DIR}/vma>)
target_precompile_headers(vma PRIVATE
                          ${PLATFORM_AGNOSTIC_VENDOR_DIR}/vma/vk_mem_alloc.h)

# tinygltf
target_include_directories(
  tinygltf
  PUBLIC $<BUILD_INTERFACE:${PLATFORM_AGNOSTIC_VENDOR_DIR}>
         $<BUILD_INTERFACE:${PLATFORM_AGNOSTIC_VENDOR_DIR}/tiny-gltf>
  INTERFACE $<INSTALL_INTERFACE:third-party>
            $<INSTALL_INTERFACE:third-party/tiny-gltf>)

# ImGui
target_include_directories(
  ImGui
  PUBLIC $<BUILD_INTERFACE:${PLATFORM_AGNOSTIC_VENDOR_DIR}>
         $<BUILD_INTERFACE:${PLATFORM_AGNOSTIC_VENDOR_DIR}/imgui>
         $<BUILD_INTERFACE:${PLATFORM_AGNOSTIC_VENDOR_DIR}/backends>
  PRIVATE $<INSTALL_INTERFACE:include>)

# Window
target_include_directories(
  Window
  PUBLIC $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
         $<BUILD_INTERFACE:${POPCORN_INCLUDE_DIR}/Core>
         $<BUILD_INTERFACE:${POPCORN_INCLUDE_DIR}/Platform>
         $<BUILD_INTERFACE:${POPCORN_INCLUDE_DIR}/Platform/Agnostic>
  INTERFACE $<INSTALL_INTERFACE:include>
            $<INSTALL_INTERFACE:include/Popcorn/Core>
            $<INSTALL_INTERFACE:include/Popcorn/Platform>
            $<INSTALL_INTERFACE:include/Popcorn/Platform/Agnostic>)

# Events
target_include_directories(
  Events
  PUBLIC $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
         $<BUILD_INTERFACE:${POPCORN_INCLUDE_DIR}/Input>
         $<BUILD_INTERFACE:${POPCORN_INCLUDE_DIR}/Events>
  INTERFACE $<INSTALL_INTERFACE:include>
            $<INSTALL_INTERFACE:include/Popcorn/Input>
            $<INSTALL_INTERFACE:include/Popcorn/Events>)

target_include_directories(
  Graphics
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Graphics>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Graphics/Materials>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Scene>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/OpenGL>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/Vulkan>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/Vulkan/Descriptors>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/Vulkan/Memory>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/Vulkan/Pipelines>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Platform/Vulkan/RenderFlows>
  INTERFACE $<INSTALL_INTERFACE:include>
            $<INSTALL_INTERFACE:include/Popcorn/Graphics>
            $<INSTALL_INTERFACE:include/Popcorn/Graphics/Materials>
            $<INSTALL_INTERFACE:include/Popcorn/Scene>
            $<INSTALL_INTERFACE:include/Popcorn/Platform/OpenGL>
            $<INSTALL_INTERFACE:include/Popcorn/Platform/Vulkan>
            $<INSTALL_INTERFACE:include/Popcorn/Platform/Vulkan/Descriptors>
            $<INSTALL_INTERFACE:include/Popcorn/Platform/Vulkan/Memory>
            $<INSTALL_INTERFACE:include/Popcorn/Platform/Vulkan/Pipelines>
            $<INSTALL_INTERFACE:include/Popcorn/Platform/Vulkan/RenderFlows>)

target_include_directories(
  Popcorn
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Core>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Loaders>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Popcorn/Layers>
  INTERFACE $<INSTALL_INTERFACE:include>
            $<INSTALL_INTERFACE:include/Popcorn/Core>
            $<INSTALL_INTERFACE:include/Popcorn/Loaders>
            $<INSTALL_INTERFACE:include/Popcorn/Layers>)

# DEBUG FLAGS
option(PC_DEBUG "USE DEBUG MODE FOR COMPILING" ON)
# option(PC_DEBUG "USE DEBUG MODE FOR COMPILING" OFF)

if(PC_DEBUG)
  target_compile_definitions(Window PUBLIC "PC_DEBUG")
  target_compile_definitions(Graphics PUBLIC "PC_DEBUG")
  target_compile_definitions(Events PUBLIC "PC_DEBUG")
  target_compile_definitions(Popcorn PUBLIC "PC_DEBUG")
endif()

if(BUILD_FOR_LINUX)
  target_compile_definitions(Window PUBLIC "PC_OS_LINUX")
  target_compile_definitions(Graphics PUBLIC "PC_OS_LINUX")
  target_compile_definitions(Events PUBLIC "PC_OS_LINUX")
  target_compile_definitions(Popcorn PUBLIC "PC_OS_LINUX")
elseif(BUILD_FOR_WIN)
  target_compile_definitions(Window PUBLIC "PC_OS_WINDOWS")
  target_compile_definitions(Graphics PUBLIC "PC_OS_WINDOWS")
  target_compile_definitions(Events PUBLIC "PC_OS_WINDOWS")
  target_compile_definitions(Popcorn PUBLIC "PC_OS_WINDOWS")
elseif(BUILD_FOR_MACOS)
  target_compile_definitions(Window PUBLIC "PC_OS_MAC")
  target_compile_definitions(Graphics PUBLIC "PC_OS_MAC")
  target_compile_definitions(Events PUBLIC "PC_OS_MAC")
  target_compile_definitions(Popcorn PUBLIC "PC_OS_MAC")
endif()

target_link_libraries(vma PUBLIC Vulkan::Headers Vulkan::Loader)
target_link_libraries(Graphics PRIVATE Vulkan::Headers Vulkan::Loader glfw vma)
target_link_libraries(Graphics PUBLIC glm::glm)

target_link_libraries(Window PRIVATE glad glfw)
target_link_libraries(Window PRIVATE Events)
target_link_libraries(ImGui PUBLIC glfw Vulkan::Headers Vulkan::Loader)
target_link_libraries(Popcorn PUBLIC tinygltf)
target_link_libraries(Popcorn PRIVATE glad glfw)

target_link_libraries(Popcorn PUBLIC Graphics)
target_link_libraries(Popcorn PRIVATE Window ImGui)

# INTERFACE LIBS
target_link_libraries(Popcorn INTERFACE glm::glm)
target_link_libraries(Popcorn PUBLIC Events)

target_compile_features(${PROJECT} PUBLIC cxx_std_20)

# -----------------------------------------------------------------------
# INSTALL CONFIG --- START
# -----------------------------------------------------------------------
include(CMakePackageConfigHelpers)

# Generate the Config file
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PopcornConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/PopcornConfig.cmake
  INSTALL_DESTINATION lib/cmake/Popcorn)

# Generate the Version file
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/PopcornConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

# Install Config and Version Files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PopcornConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/PopcornConfigVersion.cmake
        DESTINATION lib/cmake/Popcorn)

# Export static libs
if(BUILD_FOR_LINUX)
  install(DIRECTORY third-party/linux/ third-party/platform-agnostic/
          DESTINATION third-party)
elseif(BUILD_FOR_WIN)
  install(DIRECTORY third-party/windows/ third-party/platform-agnostic/
          DESTINATION third-party)
endif()

# Popcorn headers
install(DIRECTORY include/ DESTINATION include)

install(
  TARGETS Popcorn
          Window
          tinygltf
          ImGui
          Graphics
          glad
          Events
          vma
  EXPORT PopcornTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# Export the Popcorn library configuration
install(
  EXPORT PopcornTargets
  FILE PopcornTargets.cmake
  NAMESPACE Popcorn::
  DESTINATION lib/cmake/Popcorn)
