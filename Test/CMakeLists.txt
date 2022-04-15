# CMakeList.txt : CMake project for Networking, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (Test)

set(CMAKE_CXX_STANDARD 20)

if (LOG_ON)
	add_compile_definitions("LOG_ON")
endif ()

# Add source to this project's executable.
add_executable (Test "Test.cpp" "Test.h")

# TODO: Add tests and install targets if needed.
