cmake_minimum_required(VERSION 3.15...4.0)

project(NES-Emulator VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC_FILES
  "${CMAKE_SOURCE_DIR}/Source/*.cpp"
  "${CMAKE_SOURCE_DIR}/Source/*.c"
  "${CMAKE_SOURCE_DIR}/Source/*.h"
  "${CMAKE_SOURCE_DIR}/Source/.hpp")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

add_executable(Emulator ${SRC_FILES})

if (WIN32)
  set_target_properties(Emulator PROPERTIES WIN32_EXECUTABLE TRUE)
else()
  message(FATAL_ERROR "Only Windows is supported!")
endif()
