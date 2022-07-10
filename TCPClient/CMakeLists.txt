# CMakeList.txt : CMake project for TCPClient, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(TCPClient)

if(MSVC)
	# fix __cplusplus macro for MSVC
	add_compile_options(
		"/Zc:__cplusplus"
		"/std:c++latest"
	)
else()
	set(CMAKE_CXX_STANDARD 20)
endif()

if (LOG_ON)
	add_compile_definitions("LOG_ON")
	message("TCPClient: LOG_ON is defined")
else()
	message("TCPClient: LOG_ON is not defined")
endif ()
if (LOG_LEVEL_VERBOSE)
	add_compile_definitions("LOG_LEVEL_VERBOSE")
	message("TCPClient: LOG_LEVEL_VERBOSE is defined")
else()
	message("TCPClient: LOG_LEVEL_VERBOSE is not defined")
endif ()
if (LOG_LEVEL_DEBUG)
	add_compile_definitions("LOG_LEVEL_DEBUG")
	message("TCPClient: LOG_LEVEL_DEBUG is defined")
else()
	message("TCPClient: LOG_LEVEL_DEBUG is not defined")
endif()


set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(NETLIB_DIR "${ROOT_DIR}/netlib")

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

include_directories(
	"${NETLIB_INCLUDES}"
	"${ROOT_DIR}"
	"${ROOT_DIR}/include"
)

add_executable (TCPClient "TCPClient.cpp" "TCPClient.h")

target_link_libraries(TCPClient netlib)
