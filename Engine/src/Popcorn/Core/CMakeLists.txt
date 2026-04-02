add_library(Core)

target_sources(
  Core
  PRIVATE Application.cpp # used for BUILD
  PUBLIC BASE_DIRS ${ENGINE_INCLUDE_DIR} FILE_SET HEADERS
         # used for INSTALL BASE_DIRS ${ENGINE_INCLUDE_DIR}
         FILES include/Popcorn/Core/Application.h)

target_include_directories(Core PUBLIC $<BUILD_INTERFACE:${ENGINE_INCLUDE_DIR}>
                                       $<INSTALL_INTERFACE:include>)
