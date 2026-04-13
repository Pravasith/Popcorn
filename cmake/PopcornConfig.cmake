include(CMakeFindDependencyMacro)
find_dependency(Threads REQUIRED)
find_dependency(VulkanLoader REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/PopcornTargets.cmake)
