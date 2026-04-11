add_library(Graphics OBJECT)

target_sources(
  Graphics
  PRIVATE Graphics.cpp # used for BUILD
  PUBLIC FILE_SET
         HEADERS
         #
         BASE_DIRS
         ${PC_ENGINE_INCLUDE_DIR}
         FILES
         ${PC_ENGINE_INCLUDE_DIR}/Popcorn/Graphics/Graphics.h
  PRIVATE FILE_SET
          privateHeaders
          TYPE
          HEADERS
          BASE_DIRS
          ${PC_ENGINE_INCLUDE_DIR}
          FILES
          ${PC_ENGINE_INCLUDE_DIR}/Popcorn/Graphics/Secret.h)

# third party stuff find_package(Vulkan REQUIRED)
target_link_libraries(Graphics PRIVATE Vulkan::Loader)

# target_include_directories( Graphics INTERFACE
# $<BUILD_INTERFACE:${PC_ENGINE_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
