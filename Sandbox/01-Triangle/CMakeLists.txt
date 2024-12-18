cmake_minimum_required(VERSION 3.22)

set(PROJECT Triangle)
project(${PROJECT} VERSION 1.0.0)
message("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# SPECIFY THE C++ STANDARD
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#

# -----------------------------------------------------------------------
# OS FLAGS --- START
# -----------------------------------------------------------------------
set(PC_HOME "${CMAKE_SOURCE_DIR}/../../Engine")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(BUILD_FOR_LINUX TRUE)
  set(CMAKE_PREFIX_PATH "${PC_HOME}/dist/linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(BUILD_FOR_WIN TRUE)
  # IN TOOLCHAIN FILE
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(BUILD_FOR_MACOS TRUE)
  # Include the toolchain file for cross-compilation
  # include(${CMAKE_TOOLCHAIN_FILE})
endif()
# -----------------------------------------------------------------------
# OS FLAGS --- END
# -----------------------------------------------------------------------

#

# -----------------------------------------------------------------------
# STATIC BUILD SETTINGS (OPTIONAL) --- START
# -----------------------------------------------------------------------
option(STATIC_BUILD "BUILD A STATIC BINARY" ${BUILD_FOR_WIN})

if(STATIC_BUILD)
  set(CMAKE_EXE_LINKER_FLAGS "-static")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" CONFIG)
  set(BUILD_SHARED_LIBS OFF)
endif()
# -----------------------------------------------------------------------
# STATIC BUILD SETTINGS (OPTIONAL) --- END
# -----------------------------------------------------------------------

#

find_package(Popcorn REQUIRED)

set(SOURCE_FILES main.cpp)
add_executable(${PROJECT} ${SOURCE_FILES})

target_link_libraries(${PROJECT} PUBLIC Popcorn::Popcorn)

# install( TARGETS Triangle EXPORT TriangleTargets ARCHIVE DESTINATION lib
# LIBRARY DESTINATION lib RUNTIME DESTINATION bin)

# EXPORT THE INCLUDE DIRECTORIES install(DIRECTORY
# ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
