add_library(Core STATIC)

target_sources(
  Core
  PRIVATE Application.cpp # used for BUILD
  PUBLIC FILE_SET
         HEADERS
         #
         BASE_DIRS
         ${PC_ENGINE_INCLUDE_DIR}
         FILES
         ${PC_ENGINE_INCLUDE_DIR}/Popcorn/Core/Application.h)

# third party stuff
#
target_link_libraries(Core PRIVATE glfw)

# target_include_directories( Core INTERFACE
# $<BUILD_INTERFACE:${PC_ENGINE_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)
