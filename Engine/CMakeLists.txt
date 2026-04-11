#
# The root CML file is in the root dir of the project. This file is
# add_subdirectory'd into the root CML.

add_subdirectory(src/Core)
add_subdirectory(src/Graphics)

add_library(${PROJECT_NAME} STATIC)

target_sources(
  ${PROJECT_NAME}
  PRIVATE src/Popcorn.cpp # used for BUILD
  PUBLIC FILE_SET HEADERS BASE_DIRS ${PC_ENGINE_INCLUDE_DIR} FILES
         ${PC_ENGINE_INCLUDE_DIR}/Popcorn/Popcorn.h)

# target_include_directories( ${PROJECT_NAME} PUBLIC
# $<BUILD_INTERFACE:${PC_ENGINE_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE #
          Core Graphics
          # Maths Events Animation
)
