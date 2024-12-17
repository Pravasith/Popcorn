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
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(BUILD_FOR_LINUX TRUE)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(BUILD_FOR_WIN TRUE)
  # Include the toolchain file for cross-compilation
  # include(${CMAKE_TOOLCHAIN_FILE})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
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

# -----------------------------------------------------------------------
# SET EXECUTABLE --- START
# -----------------------------------------------------------------------
set(SOURCE_FILES main.cpp)
add_executable(${PROJECT} ${SOURCE_FILES})
# -----------------------------------------------------------------------
# SET EXECUTABLE --- END
# -----------------------------------------------------------------------

#

set(PC_HOME "${CMAKE_SOURCE_DIR}/../../Engine")
set(PC_INCLUDES "${PC_HOME}/include")
message("${PC_INCLUDES}")

set(PC_BIN_LINUX "${PC_HOME}/build/linux")
set(PC_BIN_WIN "${PC_HOME}/build/windows")

#

target_include_directories(
  ${PROJECT}
  PUBLIC ${PC_INCLUDES}
         ${PC_INCLUDES}/Popcorn
         ${PC_INCLUDES}/Popcorn/Core
         ${PC_INCLUDES}/Popcorn/Layers
         ${PC_INCLUDES}/Popcorn/Events
         ${PC_INCLUDES}/Popcorn/Graphics
         ${PC_INCLUDES}/Popcorn/Input
         ${PC_INCLUDES}/Popcorn/Platform)

# target_link_libraries( ${PROJECT} PUBLIC "${PC_BIN_LINUX}/src/libPopcorn.a"
# "${PC_BIN_LINUX}/src/libEvents.a" "${PC_BIN_LINUX}/src/libGraphics.a"
# "${PC_BIN_LINUX}/src/libWindow.a")
