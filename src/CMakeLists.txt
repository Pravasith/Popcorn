set(CORE_IMPL_INCLUDE_DIR Popcorn/core)

add_library(Window ${CORE_IMPL_INCLUDE_DIR}/Window.cpp)
add_library(PopcornEngine ${CORE_IMPL_INCLUDE_DIR}/Application.cpp)
