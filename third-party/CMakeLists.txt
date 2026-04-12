#
# ----------------------------- GLFW -------------------------------------

if(NOT GLFW_DIR)
  set(GLFW_DIR ${CMAKE_SOURCE_DIR}/third-party/glfw)
endif()

# set(GLFW_LIBRARY_TYPE OBJECT)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)

add_subdirectory(${GLFW_DIR} glfw/binary_dir # EXCLUDE_FROM_ALL
)
include_directories(${GLFW_DIR}/include)
# include_directories(${GLFW_DIR}/deps)

#
# -------------------------- Vulkan Loader -------------------------------
if(NOT VULKAN_LOADER_DIR)
  set(VULKAN_LOADER_DIR ${CMAKE_SOURCE_DIR}/third-party/vulkan-loader)
endif()

set(UPDATE_DEPS ON) # automatically installs vulkanheaders inside
                    # vulkan-loaders, so no need for adding below line
# set(VULKAN_HEADERS_INSTALL_DIR "/usr/share/cmake/VulkanHeaders")

add_subdirectory(
  ${VULKAN_LOADER_DIR} vulkan-loader/binary_dir # EXCLUDE_FROM_ALL
)
