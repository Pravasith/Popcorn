#
# The root CML file is in the root dir of the project. This file is
# add_subdirectory'd into the root CML.
set(PC_ENGINE_SRC_DIR ${CMAKE_SOURCE_DIR}/Engine)
set(PC_EDITOR_SRC_DIR ${CMAKE_SOURCE_DIR}/Editor)

add_subdirectory(src)

add_library(Popcorn STATIC)
target_link_libraries(Popcorn PRIVATE Core Maths Events Graphics Animation)
