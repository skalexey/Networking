# CMakeList.txt : CMake project for TCPServer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(TCPServer)

set(CMAKE_CXX_STANDARD 20)

if (LOG_ON)
	add_compile_definitions("LOG_ON")
endif ()

include_directories("C:/lib/asio-1.22.1/include")

set(NETWORKING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(NETLIB_DIR "${NETWORKING_DIR}/netlib/")

include_directories("${NETLIB_DIR}")
include_directories("${NETWORKING_DIR}")
include_directories("${NETWORKING_DIR}/include")

message("TCPServer NETLIB_DIR: '${NETLIB_DIR}'")

if (NETLIB_EXTERNAL)
else()
	if (CMAKE_CXX_COMPILER EQUAL "g++")
		set(NETLIB_BUILD_DIR_NAME "Build-g++")
	else ()
		set(NETLIB_BUILD_DIR_NAME "Build-cmake")
	endif ()
	add_subdirectory("${NETLIB_DIR}" "${NETLIB_DIR}/${NETLIB_BUILD_DIR_NAME}")
endif()

add_executable (TCPServer "TCPServer.cpp" "TCPServer.h")

target_link_libraries(TCPServer netlib)
