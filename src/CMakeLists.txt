set(CORE_IMPL_DIR Popcorn/Core)
set(EVENTS_IMPL_DIR Popcorn/Events)
set(PLATFORM_IMPL_DIR Popcorn/Platform)

add_library(PopcornEngine ${CORE_IMPL_DIR}/Application.cpp)

add_library(Window ${CORE_IMPL_DIR}/Window.cpp
                   ${PLATFORM_IMPL_DIR}/Window_Win64_Linux.cpp)

add_library(Events ${EVENTS_IMPL_DIR}/Publisher.cpp)
