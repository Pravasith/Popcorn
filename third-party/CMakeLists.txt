#
# ----------------------------- GLFW -------------------------------------

if(NOT GLFW_DIR)
  set(GLFW_DIR ${CMAKE_SOURCE_DIR}/third-party/glfw)
endif()

set(GLFW_LIBRARY_TYPE OBJECT)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)
