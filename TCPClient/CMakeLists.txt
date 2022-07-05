# CMakeList.txt : CMake project for TCPClient, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(TCPClient)

set(CMAKE_CXX_STANDARD 20)

if (LOG_ON)
	add_compile_definitions("LOG_ON")
endif ()

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(NETLIB_DIR "${ROOT_DIR}/netlib")
include_directories(
	"${NETLIB_INCLUDES}"
	"${ROOT_DIR}"
	"${ROOT_DIR}/include"
)

message("TCPClient NETLIB_DIR: '${NETLIB_DIR}'")

if (NETLIB_EXTERNAL)
else()
	if (CMAKE_CXX_COMPILER EQUAL "g++")
		set(NETLIB_BUILD_DIR_NAME "Build-g++")
	else ()
		set(NETLIB_BUILD_DIR_NAME "Build-cmake")
	endif ()
	add_subdirectory("${NETLIB_DIR}" "${NETLIB_DIR}/${NETLIB_BUILD_DIR_NAME}")
endif()


add_executable (TCPClient "TCPClient.cpp" "TCPClient.h")

target_link_libraries(TCPClient netlib)
