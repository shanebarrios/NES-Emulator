cmake_minimum_required (VERSION 3.8)

project ("NES-Emulator")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable (NES-Emulator "Source/Main.cpp"  "Source/Bus.h" "Source/CPU.h" "Source/CPU.cpp" "Source/Common.h" "Source/Bus.cpp")

