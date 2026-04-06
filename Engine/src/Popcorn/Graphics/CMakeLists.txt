add_library(Graphics STATIC)

target_sources(
  Graphics
  PRIVATE Graphics.cpp # used for BUILD
  PUBLIC BASE_DIRS ${ENGINE_INCLUDE_DIR} FILE_SET HEADERS
         # used for INSTALL BASE_DIRS ${PC_ENGINE_INCLUDE_DIR}
         FILES include/Popcorn/Graphics/Graphics.h)

target_include_directories(
  Graphics PUBLIC $<BUILD_INTERFACE:${ENGINE_INCLUDE_DIR}>
                  $<INSTALL_INTERFACE:include>)
