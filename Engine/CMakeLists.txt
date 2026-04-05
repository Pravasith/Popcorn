#
# The root CML file is in the root dir of the project. This file is
# add_subdirectory'd into the root CML.

add_subdirectory(src/Core)
add_subdirectory(src/Graphics)

add_library(Popcorn STATIC)

target_link_libraries(
  Popcorn
  PRIVATE #
          Core Graphics
          # Maths Events Animation
)
