add_library(Core STATIC)

target_sources(
  Core
  PRIVATE Application.cpp # used for BUILD
  PUBLIC BASE_DIRS ${PC_ENGINE_INCLUDE_DIR} FILE_SET HEADERS
         # used for INSTALL BASE_DIRS ${PC_ENGINE_INCLUDE_DIR}
         FILES include/Popcorn/Core/Application.h)

target_include_directories(
  Core PUBLIC $<BUILD_INTERFACE:${PC_ENGINE_INCLUDE_DIR}>
              $<INSTALL_INTERFACE:include>)
