set(CORE_IMPL_INCLUDE_DIR Popcorn/Core)
set(EVENTS_IMPL_INCLUDE_DIR Popcorn/Events)
set(PLATFORM_IMPL_INCLUDE_DIR Popcorn/Platform)

add_library(PopcornEngine ${CORE_IMPL_INCLUDE_DIR}/Application.cpp)

add_library(Window ${CORE_IMPL_INCLUDE_DIR}/Window.cpp
                   ${PLATFORM_IMPL_INCLUDE_DIR}/Window_Win64_Linux.cpp)

add_library(Events ${EVENTS_IMPL_INCLUDE_DIR}/Publisher.cpp)
