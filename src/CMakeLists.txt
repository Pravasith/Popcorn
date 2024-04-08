set(CORE_IMPL_INCLUDE_DIR engine/core)

add_library(Engine ${CORE_IMPL_INCLUDE_DIR}/Engine.cpp)
add_library(Window ${CORE_IMPL_INCLUDE_DIR}/Window.cpp)
