#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "glm::glm" for configuration ""
set_property(TARGET glm::glm APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(glm::glm PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libglm.a"
  )

list(APPEND _cmake_import_check_targets glm::glm )
list(APPEND _cmake_import_check_files_for_glm::glm "${_IMPORT_PREFIX}/lib/libglm.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
